#include "read.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "roots.h"
#include "scan.h"
#include "test.h"
#include "types.h"

/*
 * This module is the Scheme reader (parser).  It recognizes all of
 * Scheme's grammar as specified in r6rs, modulo the scanner's
 * limitations.  This includes datum comments, e.g., "#; datum", which
 * are the most difficult part.
 * 
 * The reader is implemented as a table-driven LL(1) parser.  The
 * table is generated at run time from a hardcoded grammar.  The
 * method and terminology closely follow Wikipedia's LL parser page:
 *
 *     http://en.wikipedia.org/wiki/LL_parser
 *
 * The reader runs in two phases.  The first phase, parse, uses the
 * stream of input tokens and the parsing table to generate a stack of
 * actions.  The second phase, build, consumes the actions, in LIFO
 * order, and builds the resultant Scheme expression data structure.
 */

#define DUMP_TABLES 0			/* If true, print generated tables. */

#if DUMP_TABLES
  #include <stdio.h>
#endif

/*
 * SIZED_ARRAY: declare a fixed-size static array.  Fail if the
 * program tries to allocate too many elements, and warn if it doesn't
 * allocate them all.
 *
 * The intention is to let you declare "SIZED_ARRAY(int, foo, 42)" and
 * fail the unit tests if foo's size changes.
 *
 *
 * SIZED_ARRAY(int, foo, 42) declares four names.
 *
 *  - foo is the array.  It contains 42 ints.
 *
 *  - alloc_foo(n) allocates n elements and returns the index of the
 *    first.
 *
 *  - next_foo(n) allocates n elements and returns a pointer to the
 *    first.
 *
 *  - foo_size is the number of elements currently allocated.
 */

#if NDEBUG
  #define IF_ASSERTIONS(x)
#else
  #define IF_ASSERTIONS(x) x
#endif

#define SIZED_ARRAY(type, var, size)					\
    static type var[(size)];						\
    static size_t var##_size = 0;					\
									\
    static size_t alloc_##var(size_t n)					\
    {									\
	size_t index = var##_size;					\
	var##_size += n;						\
	ASSERT(var##_size <= (size));					\
	return index;							\
    }									\
									\
    static inline type *next_##var(size_t n)				\
    {									\
        return &var[alloc_##var(n)];					\
    }									\
									\
    IF_ASSERTIONS(							\
	__attribute__((destructor))					\
	static void verify_size_##var(void)				\
	{								\
	    if (var##_size != (size)) {					\
		fprintf(stderr, __FILE__ ":%d: warning: "		\
				"array `" #var "' allocated %d, "	\
				 "used %zd %s.\n",			\
				 __LINE__, (size), var##_size,		\
				 var##_size == 1 ? "entry" : "entries");\
	    }								\
	}								\
    )

/*
 * A stack is implemented as a linked list.
 */

static inline bool stack_is_empty(const obj_t stack)
{
    return is_null(stack);
}

static inline void stack_push(obj_t *stack, obj_t elem)
{
    *stack = make_pair(elem, *stack);
}

static inline obj_t stack_pop(obj_t *stack)
{
    obj_t elem = pair_car(*stack);
    *stack = pair_cdr(*stack);
    return elem;
}

static inline obj_t stack_top(obj_t stack)
{
    return pair_car(stack);
}

#define ACTION_BEGIN_LIST	(READER_CONSTANT(0))
#define ACTION_BEGIN_VECTOR	(READER_CONSTANT(1))
#define ACTION_BEGIN_BYTEVEC	(READER_CONSTANT(2))
#define ACTION_END_SEQUENCE	(READER_CONSTANT(3))
#define ACTION_DOT_END		(READER_CONSTANT(4))
#define ACTION_ABBREV		(READER_CONSTANT(5))
#define ACTION_DISCARD		(READER_CONSTANT(6))

/*
 * Let's define some character classes and indices.
 *
 * Terminals are the symbols that are returned by the scanner.  They
 * correspond to tokens, including the special token, TOK_EOF.
 *
 * Extended terminals, or exterminals, are the set of terminals plus
 * the empty string symbol, '-'.  (It's a lowercase epsilon in the
 * literature, but we're using ASCII here.)
 *
 * Nonterminals are the symbols that appear on the left side of
 * grammar productions.  They do not appear in the input stream.
 *
 * Each class has its own index type for indexing the various parser
 * generator tables.
 *
 * Each symbol has symbol index.  All symbols, whether they're
 * terminals, nonterminals, or epsilon, have a symbol index.
 *
 * Each exterminal has a terminal index.  An exterminal's terminal
 * index is equal to its symbol index.
 *
 * Each nonterminal has a nonterminal index.  A nonterminal index
 * is NOT equal to a symbol index.
 *
 * Another way to say that is that the symbols are ordered with the
 * terminals coming first, then epsilon, then the nonterminals last.
 */


/*
 * A character type is one of (CT_TERMINAL, CT_NONTERMINAL, CT_NONE)
 * or'ed with a symbol index.
 */
typedef enum char_type {
    CT_NONE = 0,
    CT_TERMINAL = 0x40,
    CT_NONTERMINAL = 0x80,
    CTMASK = 0xC0,
    SYMMASK = ~(CTMASK)
} char_type_t;

typedef uint_fast16_t exterminal_set_t;

/*
 * The language grammar is made up of productions.  Each production
 * has a nonterminal on the left-hand side and a sequence of zero or
 * more symbols on the right-hand side.  Some productions also have
 * actions associated - something the parser should do when it begins
 * to match the production.
 *
 * For example, the production, "E ::= E + T" would be represented by
 * the production_t initializer { 'E', 'E+T', (some action) }.
 */
typedef struct production {
    char        p_lhs;
    const char *p_rhs;
    int         p_action;
} production_t;

/*
 * A token_pair pairs a token_type_t, as returned by the scanner,
 * with a terminal symbol, as used in the grammar.
 */
typedef struct token_pair {
    char         tm_term;
    token_type_t tm_ttype;
} token_pair_t;

/*
 * The grammar is a set of productions and associated actions.
 */
static const production_t grammar[] = {
    { 'p', "xp",                        }, /* program ::= comment program */
    { 'p', "d",                         }, /* program ::= datum */
    { 'p', "",                          }, /* program ::= (empty) */

    { 'd', "N",                         }, /* datum ::= EXACT_NUMBER */
    { 'd', "S",                         }, /* datum ::= SIMPLE */
    { 'd', "(i)",  ACTION_BEGIN_LIST    }, /* datum ::= ( interior ) */
    { 'd', "[i]",  ACTION_BEGIN_LIST    }, /* datum ::= [ interior ] */
    { 'd', "Ve)",  ACTION_BEGIN_VECTOR  }, /* datum ::= #( elements ) */
    { 'd', "Bb)",  ACTION_BEGIN_BYTEVEC }, /* datum ::= #vu8( bytes ) */
    { 'd', "Ad",   ACTION_ABBREV        }, /* datum ::= ABBREV datum */

    { 'i', "dj",                        }, /* interior ::= datum tail */
    { 'i', "xi",                        }, /* interior ::= comment interior */
    { 'i', "",     ACTION_END_SEQUENCE  }, /* interior ::= (empty) */

    { 'j', "dj",                        }, /* tail ::= datum tail */
    { 'j', "xj",                        }, /* tail ::= comment tail */
    { 'j', ".ydy", ACTION_DOT_END    }, /* tail ::= . comments datum comments */
    { 'j', "",     ACTION_END_SEQUENCE  }, /* tail ::= (empty) */

    { 'e', "de",                        }, /* elements ::= datum elements */
    { 'e', "xe",                        }, /* elements ::= comment elements */
    { 'e', "",     ACTION_END_SEQUENCE  }, /* elements ::= (empty) */

    { 'b', "Nb",                        }, /* bytes ::= EXACT_NUMBER bytes */
    { 'b', "xb",                        }, /* bytes ::= comment bytes */
    { 'b', "",     ACTION_END_SEQUENCE  }, /* bytes ::= (empty) */

    { 'x', ";d",   ACTION_DISCARD       }, /* comment ::= #; datum */

    { 'y', "xy",                        }, /* comments ::= comment comments */
    { 'y', "",                          }, /* comments ::= (empty) */
};
static const size_t grammar_size = sizeof grammar / sizeof *grammar;

/*
 * token_pairs map between the grammar's token symbols and the
 * token_type_t values that the scanner returns.
 */
static token_pair_t token_pairs[] = {
    { 'N', TOK_EXACT_NUMBER },
    { 'S', TOK_SIMPLE },
    { 'A', TOK_ABBREV },
    { ';', TOK_COMMENT },
    { 'V', TOK_BEGIN_VECTOR },
    { 'B', TOK_BEGIN_BYTEVECTOR },
    { '(', TOK_LPAREN },
    { ')', TOK_RPAREN },
    { '.', TOK_PERIOD },
    { '[', TOK_LBRACKET },
    { ']', TOK_RBRACKET },
    { '$', TOK_EOF },
};
static size_t token_pairs_size = sizeof token_pairs / sizeof *token_pairs;

/*
 * Some parsing table entries are empty.  They contain the NO_RULE value.
 */
static const uint_fast8_t NO_RULE = UINT8_MAX;

/*
 * For each ASCII (not Unicode) character, charmap classifies
 * the character as a terminal, nonterminal, or neither.
 * For symbols, charmap also has the char's symbol index.
 * See char_type_t above.
 */
static uint_fast8_t charmap[256];
static const size_t charmap_size = sizeof charmap / sizeof *charmap;

static inline bool char_is_nonterminal(char c)
{
    return (charmap[(uint_fast8_t)c] & CTMASK) == CT_NONTERMINAL;
}

static char start_symbol;
static size_t epsilon;
static size_t terminals_size;
static size_t exterminals_size;
static size_t nonterminals_size;

/*
 * Declare the parser generator's tables statically.
 *
 * symbols maps a symbol index to its ASCII representation.
 * sym_first maps a symbol index to the set of exterminals in FIRST(s).
 * follow maps a nonterminal index to the set of exterminals in FOLLOW(w).
 * parsing_table maps a (terminal, nonterminal) pair to a rule index.
 */
#define NT   12				/* number of terminal symbols */
#define NXT (NT + 1)			/* number of terminal symbols */
#define NN    8				/* number of nonterminal symbols */
#define NS  (NXT + NN)			/* number of symbols */
#define NPE (NT * NN)			/* number of parsing table entries */

SIZED_ARRAY(char,             symbols,           NS);
SIZED_ARRAY(exterminal_set_t, sym_first,         NS);
SIZED_ARRAY(exterminal_set_t, follow,            NN);
SIZED_ARRAY(uint_fast8_t,     parsing_table,     NPE);

/*
 * Initialize charmap and symbols.
 * symbols maps symbol indices to ASCII chars (not Unicode).
 * charmap maps unsigned chars to symbol indices.
 *
 * Also initialize start_symbol, epsilon, terminals_size,
 * exterminals_size, and nonterminals_size.  start_symbol is the
 * symbol where parsing starts (the symbol for the whole program).
 * epsilon is the symbol index of epsilon, the symbol for the empty
 * string.  *_size is the number of symbols in each set.
 */

static void init_symbols(void)
{
    size_t i, j;
    const char *p;

    start_symbol = grammar[0].p_lhs;
    for (i = 0; i < charmap_size; i++)
	charmap[i] = CT_NONE;
    for (i = 0; i < grammar_size; i++)
	charmap[(uint_fast8_t)grammar[i].p_lhs] = CT_NONTERMINAL;
    for (i = 0; i < grammar_size; i++)
	for (p = grammar[i].p_rhs; *p; p++)
	    if (charmap[(uint_fast8_t)*p] == CT_NONE)
		charmap[(uint_fast8_t)*p] = CT_TERMINAL;
    ASSERT(charmap['$'] == CT_NONE);
    charmap['$'] = CT_TERMINAL;
    ASSERT(charmap['-'] == CT_NONE);
    for (i = j = 0; i < token_pairs_size; i++, j++) {
	token_pair_t *tpp = &token_pairs[i];
	ASSERT(tpp->tm_ttype < token_pairs_size);
	uint_fast8_t *cmp = &charmap[(uint_fast8_t)tpp->tm_term];
	ASSERT(*cmp == CT_TERMINAL);
	*cmp |= tpp->tm_ttype;
	*next_symbols(1) = tpp->tm_term;
    }
    terminals_size = symbols_size;
    charmap['-'] = CT_TERMINAL | j++;
    epsilon = symbols_size;
    *next_symbols(1) = '-';
    exterminals_size = symbols_size;
    for (i = 0; i < charmap_size; i++)
	if (charmap[i] == CT_NONTERMINAL) {
	    charmap[i] |= j++;
	    *next_symbols(1) = i;
	}
    nonterminals_size = symbols_size - exterminals_size;
    ASSERT(symbols_size < CTMASK);

#if DUMP_TABLES
    printf("start_symbol = '%c'\n", start_symbol);
    printf("terminals_size = %d\n", terminals_size);
    printf("exterminals_size = %d\n", exterminals_size);
    printf("nonterminals_size = %d\n", nonterminals_size);
    printf("symbols_size = %d\n", symbols_size);
    printf("epsilon = %d\n", epsilon);
    printf("charmap\n");
    for (i = 0; i < charmap_size; i++)
	if (charmap[i])
	    printf("   ['%c'] = 0x%x\n", i, charmap[i]);
    printf("\n");

    printf("symbols\n");
    for (i = 0; i < symbols_size; i++)
	printf("   %2d: %c\n", i, symbols[i]);
    printf("\n");
#endif
}

/* map a character to its symbol index. */
static inline size_t sym_index(char sym)
{
    uint_fast8_t cm = charmap[(size_t)sym];
    ASSERT(cm & CTMASK);
    return cm & SYMMASK;
}

/* map a character to its terminal index. */
static inline size_t term_index(char term)
{
    uint_fast8_t cm = charmap[(size_t)term];
    ASSERT(cm & CT_TERMINAL);
    return cm & SYMMASK;
}

/* map a character to its nonterminal index. */
static inline size_t nonterm_index(char nonterm)
{
    uint_fast8_t cm = charmap[(size_t)nonterm];
    ASSERT((cm & CTMASK) == CT_NONTERMINAL);
    return (cm & SYMMASK) - exterminals_size;
}

/* map a terminal index to its representation character. */
static inline char terminal(size_t term_index)
{
    ASSERT(term_index < terminals_size);
    return symbols[term_index];
}

/* map a nonterminal index to its representation character. */
static inline char nonterminal(size_t nonterm_index)
{
    ASSERT(nonterm_index < nonterminals_size);
    return symbols[nonterm_index + exterminals_size];
}

/*
 * sym_first maps each symbol (including $ and epsilon) to its
 * "first-set", the set of terminals (plus epsilon) that may be first
 * in a string derived from the symbol.
 *
 * A terminal's first-set just contains the terminal.
 *
 * A nonterminal's first-set is constructed by repeatedly applying
 * productions to the symbol, finding all terminals that can be
 * reached.  See the Wikipedia article referenced above for the
 * details.
 */

static exterminal_set_t first(const char *w);

static void init_first(void)
{
    size_t i, j;
    for (i = 0; i < exterminals_size; i++)
	*next_sym_first(1) = 1 << i;
    for (i = 0; i < nonterminals_size; i++)
	*next_sym_first(1) = 0;
    for (i = 0; i < grammar_size; i++)
	if (!*grammar[i].p_rhs)
	    sym_first[sym_index(grammar[i].p_lhs)] |= 1 << epsilon;
    bool done = false;
    while (!done) {
	done = true;
	for (i = 0; i < grammar_size; i++) {
	    const production_t *pp = &grammar[i];
	    exterminal_set_t *x = &sym_first[sym_index(pp->p_lhs)];
	    for (j = 0; pp->p_rhs[j]; j++) {
		exterminal_set_t *y = &sym_first[sym_index(pp->p_rhs[j])];
		if (*y & ~*x) {
		    ASSERT(*x != (*x | *y));
		    *x |= *y;
		    done = false;
		}
		if (!(*y & 1 << epsilon))
		    break;
	    }
	}
    }

#if DUMP_TABLES
    printf("sym_first\n");
    for (i = 0; i < symbols_size; i++) {
	printf("  %c:", symbols[i]);
	exterminal_set_t s = sym_first[i];
	for (j = 0; j < exterminals_size; j++)
	    if (s & 1 << j)
		printf(" %c", symbols[j]);
	    printf("\n");
    }
    printf("\n");

    printf("first\n");
    for (i = 0; i < grammar_size; i++) {
	const production_t *pp = &grammar[i];
	printf("   %c=%-5s:", pp->p_lhs, pp->p_rhs);
	exterminal_set_t f = first(pp->p_rhs);
	for (j = 0; j < exterminals_size; j++)
	    if (f & 1 << j)
		printf(" %c", symbols[j]);
	    printf("\n");
	
    }
    printf("\n");
#endif
}

/* first(omega) returns the first-set of the symbol sequence omega. */
static exterminal_set_t first(const char *w)
{
    const char *p;
    exterminal_set_t f = 1 << epsilon;
    for (p = w; *p; p++) {
	exterminal_set_t yf = sym_first[sym_index(*p)];
	f |= yf;
	if (!(yf & 1 << epsilon)) {
	    f &= ~(1 << epsilon);
	    break;
	}
    }
    return f;
}

/*
 * Initialize follow.  follow maps each nonterminal to its follow-set.
 *
 * A symbol's follow-set is the set of terminals (including $,
 * excluding epsilon) that could follow the expansion of the
 * nonterminal.  See Wikipedia again.
 */
static void init_follow(void)
{
    int i, j;
    const char *p;

    for (i = 0; i < nonterminals_size; i++)
	*next_follow(1) = 0;
    follow[nonterm_index(start_symbol)] = 1 << sym_index('$');
    for (i = 0; i < grammar_size; i++)
	for (p = grammar[i].p_rhs; *p; p++) {
	    char b = *p;
	    const char *beta = p + 1;
	    if (char_is_nonterminal(b))
		follow[nonterm_index(b)] |= first(beta) & ~(1 << epsilon);
	}
    bool done = false;
    while (!done) {
	done = true;
	for (i = 0; i < grammar_size; i++) {
	    const production_t *pp = &grammar[i];
	    char a = pp->p_lhs;
	    exterminal_set_t *fa = &follow[nonterm_index(a)];
	    for (j = strlen(pp->p_rhs); --j >= 0; ) {
		char b = pp->p_rhs[j];
		const char *beta = pp->p_rhs + j + 1;
		if (!(first(beta) & 1 << epsilon))
		    break;
		if (char_is_nonterminal(b)) {
		    exterminal_set_t *fb = &follow[nonterm_index(b)];
		    if (*fa & ~*fb) {
			done = false;
			*fb |= *fa;
		    }
		}
	    }
	}
    }

#if DUMP_TABLES
    printf("follow\n");
    for (i = 0; i < nonterminals_size; i++) {
	printf("   %c:", nonterminal(i));
	for (j = 0; j < symbols_size; j++)
	    if (follow[i] & 1 << j)
		printf(" %c", symbols[j]);
	printf("\n");
    }
    printf("\n");
#endif
}

/*
 * Calculate the parsing table entry for a (nonterminal, terminal) pair.
 * See Wikipedia again.
 */
static int pt_entry(char A, char a)
{
    int found = NO_RULE;
    size_t ia = sym_index(a);
    exterminal_set_t foA = follow[nonterm_index(A)];
    bool a_follows_A = (bool)(foA & 1 << ia);
    size_t i;
    for (i = 0; i < grammar_size; i++) {
	const production_t *pp = &grammar[i];
	if (pp->p_lhs == A) {
	    exterminal_set_t fig = first(pp->p_rhs);
	    if (fig & 1 << ia) {
		ASSERT(found == NO_RULE && "grammar not LL(1)");
		found = i;
	    }
	    if (a_follows_A && (fig & 1 << epsilon)) {
		ASSERT(found == NO_RULE && "grammar not LL(1)");
		found = i;
	    }		
	}
    }
    return found;
}

static uint_fast8_t parsing_table_entry(size_t i, size_t j)
{
    ASSERT(i < nonterminals_size);
    ASSERT(j < terminals_size);
    return parsing_table[i * terminals_size + j];
}

/*
 * Construct the parsing table.  The parsing table is logically a 2D
 * array, indexed by a nonterminal index and a terminal index.
 * The [N, T] entry contains a rule index (index into grammar[]).
 * See Wikipedia again.
 */
static void init_parsing_table(void)
{
    size_t i, j;

    for (i = 0; i < nonterminals_size; i++) {
	char A = nonterminal(i);
	for (j = 0; j < terminals_size; j++) {
	    char a = terminal(j);
	    *next_parsing_table(1) = pt_entry(A, a);
	}
    }
    
#if DUMP_TABLES
    printf("parsing table\n");
    printf("    ");
    for (j = 0; j < TOKEN_TYPE_COUNT; j++)
	printf("%3c", terminal(j));
    printf("\n");
    printf("    ");
    for (j = 0; j < terminals_size; j++)
	printf("---");
    printf("\n");
    for (i = 0; i < nonterminals_size; i++) {
	printf("%3c :", nonterminal(i));
	for (j = 0; j < terminals_size; j++) {
	    uint_fast8_t e = parsing_table_entry(i, j);
	    if (e == NO_RULE)
		printf(" - ");
	    else
		printf("%3d", e);
	}
	printf("\n");
    }

    printf("\n");
#endif    
}

__attribute__((constructor))
static void init_parser(void)
{
    init_symbols();
    init_first();
    init_follow();
    init_parsing_table();
}

/* Initialization code is above this point.  Code below runs at parse time. */

static uint_fast8_t get_rule(char symbol, size_t term)
{
    uint_fast8_t rule = NO_RULE;
    if (char_is_nonterminal(symbol))
	rule = parsing_table_entry(nonterm_index(symbol), term);
    return rule;
}

/*
 * Parse the input stream and return an action stack.
 * See Wikipedia again.
 */
static obj_t parse(instream_t *in)
{
    obj_t actions = EMPTY_LIST;
    obj_t yylval = EMPTY_LIST;
    obj_t tmp = make_fixnum(TOK_EOF);
    obj_t stack = EMPTY_LIST;
    stack_push(&stack, tmp);
    tmp = make_fixnum(sym_index(start_symbol));
    stack_push(&stack, tmp);
    int tok = yylex(&yylval, in);
    while (true) {
	int sym = fixnum_value(stack_pop(&stack));
	ASSERT(0 <= sym && sym < symbols_size);
	uint_fast8_t rule = get_rule(symbols[sym], tok);
	if (rule != NO_RULE) {
	    const production_t *pp = &grammar[rule];
	    int j;
	    for (j = strlen(pp->p_rhs); --j >= 0; ) {
		tmp = make_fixnum(sym_index(pp->p_rhs[j]));
		stack_push(&stack, tmp);
	    }
	    if (pp->p_action)
		stack_push(&actions, (obj_t)pp->p_action);
	} else {
	    if (sym == TOK_EOF)
		break;
	    if (sym != tok)
		raise(&syntax, make_fixnum(tok), "syntax error", sym);
	    if (!is_null(yylval))
		stack_push(&actions, yylval);
	    if (!stack_is_empty(actions) &&
		fixnum_value(stack_top(stack)) == TOK_EOF)
		break;
	    yylval = EMPTY_LIST;
	    tok = yylex(&yylval, in);
	}
    }
    return actions;
}

/* Build a Scheme expression from an action stack. */
static bool build(obj_t actions, obj_t *obj_out)
{
    obj_t vstack = EMPTY_LIST;
    obj_t reg = EMPTY_LIST;
    obj_t tmp = EMPTY_LIST;
    while (!stack_is_empty(actions)) {
	obj_t op = stack_pop(&actions);
	switch ((word_t)op) {

	default:
	    reg = make_pair(op, reg);
	    continue;

	case ACTION_BEGIN_LIST:
	    reg = make_pair(reg, stack_pop(&vstack));
	    continue;

	case ACTION_BEGIN_VECTOR:
	    reg = make_vector_from_list(reg);
	    reg = make_pair(reg, stack_pop(&vstack));
	    continue;

	case ACTION_BEGIN_BYTEVEC:
	    reg = make_bytevector_from_list(reg);
	    reg = make_pair(reg, stack_pop(&vstack));
	    continue;

	case ACTION_ABBREV:
	    tmp = make_pair(pair_cadr(reg), EMPTY_LIST);
	    tmp = make_pair(pair_car(reg), tmp);
	    reg = make_pair(tmp, pair_cddr(reg));
	    continue;

	case ACTION_END_SEQUENCE:
	    stack_push(&vstack, reg);
	    reg = EMPTY_LIST;
	    continue;

	case ACTION_DOT_END:
	    stack_push(&vstack, pair_cdr(reg));
	    reg = pair_car(reg);
	    continue;

	case ACTION_DISCARD:
	    reg = pair_cdr(reg);
	    continue;
	}
    }
    ASSERT(stack_is_empty(vstack));

    bool success = false;
    if (!is_null(reg)) {
	ASSERT(is_null(pair_cdr(reg)));
	*obj_out = pair_car(reg);
	success = true;
    }
    return success;
}

bool read_stream(instream_t *in, obj_t *obj_out)
{
    return build(parse(in), obj_out);
}

/* lists */
TEST_READ(L"(a b)",			L"(a b)");
TEST_EVAL(L"(pair? '(a b))",		L"#t");
TEST_READ(L"[a b]",			L"(a b)");
TEST_EVAL(L"(pair? '[a b])",		L"#t");

/* vectors */
TEST_READ(L"#(a b)",			L"#(a b)");
TEST_READ(L"#(a (b c))",		L"#(a (b c))");
TEST_READ(L"#(a #(b c))",		L"#(a #(b c))");

/* bytevectors */
TEST_READ(L"#vu8()",			L"#vu8()");
TEST_READ(L"#vu8(1 2)",			L"#vu8(1 2)");

/* abbreviations */
TEST_READ(L"'a",			L"(quote a)");
TEST_READ(L"'(a b)",			L"(quote (a b))");
TEST_READ(L"#('a '(a b))",		L"#((quote a) (quote (a b)))");
TEST_READ(L"('a b c)",			L"((quote a) b c)");
TEST_READ(L"(a 'b c)",			L"(a (quote b) c)");
TEST_READ(L"(a b 'c)",			L"(a b (quote c))");
TEST_READ(L"'''a",                      L"(quote (quote (quote a)))");
TEST_READ(L"`a",			L"(quasiquote a)");
TEST_READ(L",a",			L"(unquote a)");
TEST_READ(L",@a",			L"(unquote-splicing a)");
TEST_READ(L"#'a",			L"(syntax a)");
TEST_READ(L"#`a",			L"(quasisyntax a)");
TEST_READ(L"#,a",			L"(unsyntax a)");
TEST_READ(L"#,@a",			L"(unsyntax-splicing a)");

/* comments */
TEST_READ(L"#; asdf ghjk",		L"ghjk");
TEST_READ(L"(a#;()b)",                  L"(a b)");
TEST_READ(L"(a#;(comment)b)",           L"(a b)");
TEST_READ(L"(a#; \t(comment) b)",       L"(a b)");
TEST_READ(L"(a#;(\n)b)",                L"(a b)");
TEST_READ(L"(a#;\t()b)",                L"(a b)");
TEST_READ(L"(a#;((c)(d))b)",            L"(a b)");
TEST_READ(L"(#;c a . b)",		L"(a . b)");
TEST_READ(L"(#;c#;c a . b)",		L"(a . b)");
TEST_READ(L"(a#;c . b)",		L"(a . b)");
TEST_READ(L"(a #;c#;c . b)",		L"(a . b)");
TEST_READ(L"(a #;c#;c . #;c b)",	L"(a . b)");
TEST_READ(L"(a #;c . #;c #;c b)",	L"(a . b)");
TEST_READ(L"(a #;c#;c . #;c b #;c)",	L"(a . b)");
TEST_READ(L"(a . #;c#;c b#;c#;c)",	L"(a . b)");
TEST_READ(L"(a#;c . #;c#;c b#;c#;c)",	L"(a . b)");
TEST_READ(L"(a . #;()#;() b#;()#;())",	L"(a . b)");
TEST_READ(L"(a #;(b #;(c d) e) f)",	L"(a f)");
TEST_READ(L"(a#!r6rs b)",		L"(a b)");
TEST_READ(L"#!r6rs(a b)",		L"(a b)");
TEST_READ(L"(#!r6rs a b)",		L"(a b)");
TEST_READ(L"(#!r6\x33s a b)",		L"(a b)");
