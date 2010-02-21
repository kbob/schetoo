#!/usr/bin/python3

from collections import defaultdict
from ply import lex, yacc
from ply.lex import TOKEN

# What's the smallest language worth scanning?
# (, ., ), symbol, signed decimal int, char, string, whitespace, ;comment

# #| is a token.  On recognizing it, eat until matching |#, then
# get another token.

# #; is a token.  Pass it to reader.


class RE(object):

    # Predefined REs: empty_set (empty set), ε (empty string)
    # RE subclasses: CC, alt, cat, kc, intersect, complement, epsilon
    # RE expressions: __or__, __mul__, __call__, __and__, __invert__
    # RE Precedence: | 0, & 1, * 2, ~ 3, () 4, CC 5, ε 6

    def __eq__(self, other):
        return self.cmp(other) == 0

    def __ne__(self, other):
        return self.cmp(other) != 0
    
    def __lt__(self, other):
        #print('lt(%.8r, %.8r) => %s' % (self, other, self.cmp(other) < 0))
        return self.cmp(other) < 0

    def __le__(self, other):
        return self.cmp(other) <= 0

    def __ge__(self, other):
        return self.cmp(other) >= 0

    def __gt__(self, other):
        return self.cmp(other) > 0

    def cmp(self, other):
        diff = self.precedence - other.precedence
        if diff < 0: return -1
        if diff > 0: return +1
        return self.cmp_like(other)

    terms = []

    def vfy_total_ordering(self, terms):
        v = sorted(terms)
        for i in range(len(v)):
            for j in range(len(v)):
                assert (v[i] == v[j]) == (i == j)
                assert (v[i] != v[j]) == (i != j)
                assert (v[i] < v[j]) == (i < j)
                assert (v[i] > v[j]) == (i > j)
                assert (v[i] <= v[j]) == (i <= j)
                assert (v[i] >= v[j]) == (i >= j)

    def __or__(self, other):
        #print('__or__(%r, %r)' % (self, other))
        #print('  self = %s %r' % (self.__class__.__name__, self))
        #print('  other = %s %r' % (other.__class__.__name__, other))
        assert       self != other
        assert not  (self == other)
        assert      (self < other) or  (self > other)
        assert not ((self < other) and (self > other))
        #print('  a < b' if self < other else '  a > b')
        if 1:
            if self not in self.terms:
                self.terms.append(self)
            if other not in self.terms:
                self.terms.append(other)
            self.vfy_total_ordering(self.terms);
            #print('  %d terms' % len(self.terms))
        if self == empty_set:           # ∅ | r ≈ r
            #print('  =>1 %r' % other)
            return other
        if other == empty_set:          # r | ∅ ≈ r
            #print('  =>2 %r' % self)
            return self
        if self == ~empty_set:          # ¬∅ | r ≈ ∅
            #print('  =>3 %r' % ~empty_set)
            return ~empty_set
        if other == ~empty_set:         # r | ¬∅ ≈ ∅
            #print('  =>4 %r' % ~empty_set)
            return ~empty_set
        if self == other:               # r | r ≈ r
            #print('  =>5 %r' % self)
            return self
        if isinstance(self, CC) and isinstance(other, CC):
            #print('  =>6 CC(%r, %r)' % (self, other))
            return CC(self, other)
        if isinstance(self, CC) and isinstance(other, alt):
            if isinstance(other.left, CC):
                #print('  =>7.5 CC(%r, %r) | %r' %
                #      (self, other.left, other.right))
                return CC(self, other.left) | other.right
            if isinstance(other.right, CC):
                #print('  =>7.6 CC(%r, %r) | %r' %
                #      (self, other.right, other.left))
                return CC(self, other.right) | other.left
        if self > other:                # s | r ≈ r | s
            #print('  =>7 %r | %r' % (other, self))
            return other | self
        if isinstance(self, alt):
            v = [self.left, self.right, other]
            sv = sorted(v)
            if v != sv:
                #print('  =>8 (%r | %r) | %r' % (sv[0], sv[1], sv[2]))
                return sv[0] | sv[1] | sv[2]
        if isinstance(other, alt):      # r | (s | t) ≈ (r | s) | t
            v = [self, other.left, other.right]
            t = ['a +-|...', 'b ->(...)*', 'c {}.{}']
            #print('SOR')
            st = sorted(t, key=lambda h: v[ord(h[0]) - ord('a')])
            sv = sorted(v)
            #print('TED %r' % st)
            if v != sv:
                #print('  =>9 (%r | %r) | %r' % (sv[0], sv[1], sv[2]))
                return sv[0] | sv[1] | sv[2]
        #print('  =>10 alt(%r, %r)' % (self, other))
        return alt(self, other)

    def __and__(self, other):
        assert False
        if self == empty_set:           # ∅ & r ≈ ε
            return empty_set
        if other == empty_set:          # r & ∅ ≈ ε
            return empty_set
        if self == ~empty_set:          # ¬∅ & r ≈ r
            return other
        if other == ~empty_set:         # r & ¬∅ ≈ r
            return self
        if other == self:               # r & r ≈ r
            return self
        if self > other:                # s & r ≈ r & s
            return other & self
        if isinstance(other, intersect): # r & (s & t) ≈ (r & s) & t
            return (self & other.left) & other.right
        return intersect(self, other)

    def __mul__(self, other):
        #print('__mul__(%r, %r)' % (self, other))
        if self == empty_set:           # ∅ * r ≈ ∅
            return empty_set
        if other == empty_set:          # r * ∅ ≈ ∅
            return empty_set
        if self == ε:                   # ε * r ≈ r
            return other
        if other == ε:                  # r * ε ≈ r
            return self
        if isinstance(other, cat):      # r * (s * t) ≈ (r * s) * t
            return (self * other.left) * other.right
        return cat(self, other)

    def __call__(self):
        if isinstance(self, kc):        # (r*)* ≈ r*
            return self
        if self == ε:                   # ε* ≈ ε
            return ε
        if self == empty_set:           # ∅* ≈ ε
            return ε
        return kc(self)
    kc = __call__

    def __invert__(self):
        if isinstance(self, complement): # ¬(¬r) ≈ r
            return self.r
        return complement(self)


class eps(RE):

    def __init__(self):
        self.precedence = 6

    def __repr__(self):
        return 'ε'

    def cmp_like(self, other):
        assert other is ε
        return 0

ε = eps()


class alt(RE):

    def __init__(self, r, s):
        self.left = r
        self.right = s
        self.precedence = 0

    def __repr__(self):
        lrep = repr(self.left)
        rrep = repr(self.right)
        if self.left.precedence < self.precedence:
            lrep = '(%s)' % lrep
        if self.right.precedence <= self.precedence:
            rrep = '(%s)' % rrep
        return '%s|%s' % (lrep, rrep)

    def cmp_like(self, other):
        assert isinstance(other, alt)
        return self.left.cmp(other.left) or self.right.cmp(other.right)


class cat(RE):

    def __init__(self, r, s):
        self.left = r
        self.right = s
        self.precedence = 2

    def __repr__(self):
        lrep = repr(self.left)
        rrep = repr(self.right)
        if self.left.precedence < self.precedence:
            lrep = '(%s)' % lrep
        if self.right.precedence <= self.precedence:
            rrep = '(%s)' % rrep
        return '%s%s' % (lrep, rrep)

    def cmp_like(self, other):
        assert isinstance(other, cat)
        return self.left.cmp(other.left) or self.right.cmp(other.right)


class complement(RE):

    def __init__(self, r):
        self.r = r
        self.precedence = 3

    def __repr__(self):
        rrep = repr(self.r)
        if r.precedence < 4:
            rrep = '(%s)' % rrep
        return '¬' + rrep

class kc(RE):

    """kc = Kleene closure"""

    def __init__(self, r):
        self.r = r
        self.precedence = 4

    def __repr__(self):
        if self.r.precedence < self.precedence:
            return '(%r)*' % (self.r)
        return '%r*' % self.r

    def cmp_like(self, other):
        assert isinstance(other, kc)
        return self.r.cmp(other.r)


class CC(RE):

    """character class"""

    all = {}
    named = {}
    def __new__(cls, *args, name=None):
        charmap = cls.__make_charmap(*args)
        try:
            return cls.all[charmap]
        except KeyError:
            new = super(CC, cls).__new__(cls)
            cls.all[charmap] = new
            return new

    def __init__(self, *args, name=None):
        # N.B., init is idempotent.
        self.precedence = 5
        self.charmap = self.__make_charmap(*args)
        self.name = name
        if name:
            self.named[self.charmap] = name

    def chars(self, ):
        return [chr(i) for i in self.ints()]
            
    def ints(self):
        return [i for i in range(self.charmap.bit_length() + 1)
                if self.charmap & (1 << i)]

    def __find_range(self, n):
        end = n
        while self.charmap & (1 << end):
            end += 1
        return end

    def __repr__(self):
        def char_repr(c):
            if ord(c) >= 128:
                return self.named_chars[c]
            return repr(c)[1:-1]
        if self.name:
            return self.name
        charmap = self.charmap
        if charmap == 0:
            return '∅'
        if not charmap & charmap - 1:
            return char_repr(chr(charmap.bit_length() - 1))
        names = []
        for i in self.named:
            if (charmap & i) == i:
                charmap &= ~i
                names.append(self.named[i])
        names.extend(char_repr(chr(i))
                     for i in range(charmap.bit_length() + 1)
                     if  charmap & (1 << i) )
        rep = ' '.join(names)
        return '{%s}' % rep
        if charmap & charmap - 1:
            chars = self.chars()
            return '{%s}' % ' '.join(char_repr(c) for c in chars)
        return char_repr(chr(charmap.bit_length() - 1))

    def cmp_like(self, other):
        assert isinstance(other, CC)
        diff = self.charmap - other.charmap
        return -1 if diff < 0 else (+1 if diff > 0 else 0)

    def XXX__or__(self, other):
        print('CC.__or__(%r, %r)' % (self, other))
        if isinstance(other, CC):
            print('  =>11 CC(%r, %r)' % (self, other))
            return CC(self, other)
        return super(CC, self).__or__(other)

    def __and__(self, other):
        if hasattr(other, 'charmap'):
            raise NotImplementedError
        return super(CC, self).__and__(other)

    @classmethod
    def __make_charmap(cls, *args):
        charmap = 0
        for arg in args:
            try:
                charmap |= arg.charmap
            except AttributeError:
                charmap |= cls.__decode_cc(arg)
        return charmap
        
    @classmethod
    def __decode_cc(cls, cc):
        if isinstance(cc, CC):
            return cc.charmap
        elif len(cc) > 1 and '-' in cc:
            a, b = cc.split('-')
            a, b = cls.__decode_c(a), cls.__decode_c(b)
            cm = 0
            for i in range(a, b + 1):
                cm |= 1 << i
            return cm
        else:
            return 1 << cls.__decode_c(cc)

    @classmethod
    def __decode_c(cls, c):
        if len(c) == 2 and c[0] == '\\':
            return {
                '\\': ord('\\'),
                'n': ord('\n'),
                }[c[1]]
        else:
            assert len(c) == 1
            return ord(c)
        
    next_cat = 128
    named_chars = {}
    catmap = {}

    @classmethod
    def unicode_cat(cls, code):
        mark = chr(cls.next_cat)
        cls.next_cat += 1
        ascii_members = cls.__ascii_by_cat(code)
        cc = cls(mark, *ascii_members, name=code)
        cls.named_chars[mark] = code
        return cc

    @classmethod
    def __ascii_by_cat(cls, code):

        if not cls.catmap:
            cls.catmap = defaultdict(list)
            with open('UnicodeData.txt') as data:
                for line in data:
                    fields = line.split(';')
                    cpt = int(fields[0], 0x10)
                    if cpt >= 128:
                        break
                    ccode = fields[2]
                    cls.catmap[ccode].append(chr(cpt))
        return cls.catmap[code]


empty_set = CC()
# print(CC('a') | ε)
Lu = CC.unicode_cat('Lu')
Ll = CC.unicode_cat('Ll')
Lt = CC.unicode_cat('Lt')
Lm = CC.unicode_cat('Lm')
Lo = CC.unicode_cat('Lo')
Mn = CC.unicode_cat('Mn')
Mc = CC.unicode_cat('Mc')
Me = CC.unicode_cat('Me')
Nd = CC.unicode_cat('Nd')
Nl = CC.unicode_cat('Nl')
No = CC.unicode_cat('No')
Pc = CC.unicode_cat('Pc')
Pd = CC.unicode_cat('Pd')
Ps = CC.unicode_cat('Ps')
Pe = CC.unicode_cat('Pe')
Pi = CC.unicode_cat('Pi')
Pf = CC.unicode_cat('Pf')
Po = CC.unicode_cat('Po')
Sm = CC.unicode_cat('Sm')
Sc = CC.unicode_cat('Sc')
Sk = CC.unicode_cat('Sk')
So = CC.unicode_cat('So')
Zs = CC.unicode_cat('Zs')
Zl = CC.unicode_cat('Zl')
Zp = CC.unicode_cat('Zp')
Cc = CC.unicode_cat('Cc')
Cf = CC.unicode_cat('Cf')
Cs = CC.unicode_cat('Cs')
Co = CC.unicode_cat('Co')
Cn = CC.unicode_cat('Cn')

def lit(s):
    x = CC(s[0])
    for c in s[1:]:
        x = cat(x, CC(c))
    return x

#z = CC('a', 'b') | CC('c', 'd') | (CC('e', 'f') | lit('xy'))
#print(z)
#exit()

letter = CC('a-z', 'A-Z', name='<letter>')
print('letter')
# constituent = CC(letter, Lu, Ll, Lt, Lm, Lo, Mn,
#                  Nl, No, Pd, Pc, Po, Sc, Sm, Sk, So, Co)
constituent = CC(letter, Lu)
print('constituent')
special_initial = CC('!', '$', '%', '&', '*', '/', ':', '<', '=',
                     '>', '?', '^', '_', '~', name='<special initial>')
print('special_initial')
digit = CC('0-9', name='<digit>')
print('digit')
hex_digit = CC(digit, 'a-f', 'A-F', name='<hex digit>')
print('hex_digit')
hex_scalar_value = hex_digit * hex_digit()
print('hex_scalar_value')
inline_hex_escape = lit(r'\x') * hex_scalar_value * lit(';')
print('inline_hex_escape')
initial = constituent | special_initial | inline_hex_escape
print('initial')
special_subsequent = CC('+', '-', '.', '@')
print('special_subsequent')
subsequent = initial | digit | CC(Nd, Mc, Me) | special_subsequent
print('subsequent')
# print(subsequent)
# exit()

if 0:
    print('\n\n')
    a = CC('+', '-')
    print('a', a)
    b = lit('...')
    print('b', b)
    c = lit('->') * subsequent()
    print('c', c)
    print('\n\n')
    print('a < a', a < a)
    print('a < b', a < b)
    print('a < c', a < c)
    print('b < a', b < a)
    print('b < b', b < b)
    print('b < c', b < c)
    print('c < a', c < a)
    print('c < b', c < b)
    print('c < c', c < c)
    print('sorted', sorted([a, b, c]))
    exit()

peculiar_identifier = CC('+') | CC('-') | lit('...') | lit('->') * subsequent()
print('peculiar_identifier')
identifier = initial * subsequent() | peculiar_identifier
print('identifier')
print(identifier)
exit()


##############################################################################


class MyLexicalError(RuntimeError):
    pass


class Token(object):
    def __init__(self, type, constructor=None, value=None):
        self.type = type
        self.constructor = constructor
        self.value = value


class Context(object):
    def __init__(self, state, buf):
        self.state = state
        self.buf = buf


class DFA(object):

    def __init__(self):
        self.transitions = [[]]
        self.delimiter_needed = []
        self.accept = []
        # self.char_class = []

    def char_class(c):
        try:
            return '\n"#\'()-.;\\ft'.index(c)
        except ValueError:
            if c in ' \t':
                return 12
            if c.isalpha():
                return 13
            if c.isdigit():
                return 14
            raise MyLexicalError('illegal char %r' % c)

    def make_token(self, state, buf):
        return self.accept[state](buf)

    def scan(c, ctx):
        state = ctx.state
        buf = ctx.buf
        tokens = []
        cc = self.char_class[c]
        if is_delimiter(cc) and self.delimiter_needed[state]:
            tokens.append(self.make_token(state, buf))
            buf = ''
        state = self.transition[state][cc]
        buf += c
        if state == self.error_state:
            raise MyLexicalError()
        if self.accept[state] and not self.delimiter_needed[state]:
            tokens.append(self.make_token(state, buf))
            buf = ''
        return Context(state, buf), tokens


def ident(buf):
    return Token('SIMPLE', 'symbol', buf)

def number(buf):
    n = int(buf)
    if 0 <= n <= 255:
        type = 'EXACT_NUMBER'
    else:
        type = 'SIMPLE'
    return Token(type, 'fixnum', buf)

def lparen(buf):
    return Token('LPAREN')

def period(buf):
    return Token('PERIOD')

def rparen(buf):
    return Token('RPAREN')

def apostrophe(buf):
    return Token('ABBREV', 'symbol', 'quote')

def string(buf):
    return Token('SIMPLE', 'string', buf[1:-1])

def character(buf):
    return Token('SIMPLE', 'character', buf[-1])

def true(buf):
    return Token('SIMPLE', 'boolean', True)

def false(buf):
    return Token('SIMPLE', 'boolean', False)

# A = alphabetic
# C = any character
# D = digit
# Q = not doublequote

patterns = (
    (r';.*\n', None),
    (r' \t\n', None),
    (r'A(A|D)*', ident),
    (r'(|-)AA*', number),
    ('(', lparen),
    ('.', period),
    (')', rparen),
    ("'", apostrophe),
    ('"Q*"', string),
    ('#\C', character),
    ('#(T|t)', true),
    ('#(F|f)', false),
    )

rstar = [p for (p, c) in patterns]

class MySyntaxError(RuntimeError):
    pass

tokens = (
    'EXACT_NUMBER',
    'SIMPLE',
    'ABBREV',
    'COMMENT',
    'BEGIN_VECTOR',
    'BEGIN_BYTEVECTOR',
    'LPAREN',
    'RPAREN',
    'PERIOD',
    'LBRACKET',
    'RBRACKET',
    'EOF',
    'ident',
    'dot_ident',
    'boolean',
    'char',
    'decimal_number',
    'nondecimal_number',
    )

t_ignore = '[\t\n\v\f\r \u0085\\s]'

# delimiter must follow ident, ., number, char, boolean.
# delimiter is optional after ( #( #vu8( ) [ ] string abbrev #; EOF
delimiter = r'(?=[()[\]";\#]|%s|\Z)' % t_ignore

t_COMMENT = r'\#;'
t_BEGIN_VECTOR = r'\#\('
t_BEGIN_BYTEVECTOR = r'\#vu8\('
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_PERIOD = r'\.' + delimiter
t_LBRACKET = r'\['
t_RBRACKET = r']'

# Things grouped under SIMPLE:
# ident .ident #t #f char number - -> ->ident -number + +number string

@TOKEN(r'[-!\$%&\*/:<=>\?^_~\w][-!\$%&\*/:<=>\?^_~\w\d]*' + delimiter)
def t_ident(t):
    t.type = 'SIMPLE'
    t.value = 'symbol(%s)' % t.value
    return t
    
@TOKEN(r'\.[-!$%&\*/:<=>\?\^_~\w\d]+' + delimiter)
def t_dot_ident(t):
    t.type = 'SIMPLE'
    t.value = 'symbol(%s)' % t.value
    return t

@TOKEN(r'\#[TtFf]' + delimiter)
def t_boolean(t):
    t.type = 'SIMPLE'
    t.value = 't' in t.value.lower()
    return t

@TOKEN(r'\#\\(?:[a-z]+|x[0-9a-fA-F]+|.)' + delimiter)
def t_char(t):
    z = t.value[2:]
    if len(z) > 1:
        if z[0] == 'x':
            z = chr(int(z[3:], 0x10))
        else:
            z = {
                'nul': '\0',
                'alarm': '\a',
                'backspace': '\b',
                'tab': '\t',
                'linefeed': '\n',
                'newline': '\n',
                'vtab': '\v',
                'page': '\f',
                'return': '\r',
                'esc': '\33',
                'space': ' ',
                'delete': '\177',
                }.get(z, None)
            if z == None:
                raise MyLexicalError(t.value)
    t.type = 'SIMPLE'
    t.value = 'character(%d)' % ord(z)
    return t

# uint = 123
# decimal = 123 | 123e4 | 123e+4 | 123e-4 | .123e4 | 1.23e4 | 123.e4
# ureal (base 10) = 123 | 123/456 | decimal | decimal|7
# ureal = 123 | 123/456
# real = ureal | +ureal | -ureal
# complex = R | R@R | R+Ui | R+nan.0i | R+inf.0i | +U+i | +nan.0i | +i
# prefix = #b | #i#b | #b#i
# prefix (base 10) = #d | #i#d | #d#i | (empty)
# number = prefix complex

def number_pattern(flag, digit_pattern):
    def expand(pat, vars):
        # vars = dict((k, vars[k]) for k in vars if len(k) == 1)
        while any(v in pat for v in vars):
            for v in vars:
                if v in pat:
                    pat = pat.replace(v, vars[v])
        return pat
    # Don't use B O D X I E S F D L
    base = r'\#[%s%s]' % (flag.upper(), flag.lower())
    prefix = r'(?:base|base\#[IiEe]|\#[IiEe]base)'
    uint = 'digit_pattern+'
    U = ureal = r'(?:uint|uint/uint)'
    if flag == 'd':
        prefix = r'(?:|base|base\#[IiEe]|\#[IiEe]base)'
        suffix = r'(?:|[EeSsFfDdLl]\d+)'
        decimal = r'(?:\d+suffix|\.\d+suffix|\d+.\d*suffix)'
        mantwid = r'(?:|\|\d+)'
        U = ureal = r'(?:uint|uint/uint|decimalmantwid)'
    R = real = r'[+-]?U'
    naninf = r'(?:nan|inf)\.0'
    complex = r'(?:R|R@R|R[+-]Ui|R[+-]naninfi|R[+-]i|[+-]Ui|[+-]naninfi|[+-]i)'
    number = prefix + complex
    return expand(number, locals())

@TOKEN('(?:%s|%s|%s)%s' % (number_pattern('b', '[01]'),
                           number_pattern('o', '[0-7]'),
                           number_pattern('x', '[\dA-Fa-f]'),
                           delimiter))
def t_nondecimal_number(t):
    z = t.value
    radix = 10
    exact = True
    while z.startswith('#'):
        f = z[1].lower()
        if f == 'e':
            exact = True
        elif f == 'i':
            exact = False
        elif f == 'b':
            radix = 2
        elif f == 'o':
            radix = 8
        elif f == 'x':
            radix = 0x10
        else:
            assert f == 'e'
        z = z[2:]
    value = int(z, radix)
    if exact:
        t.value = value
        t.type = 'EXACT_NUMBER' if 0 <= value <= 255 else 'SIMPLE'
    else:
        t.value = float(value)
        t.type = 'SIMPLE'


@TOKEN(number_pattern('d', r'\d') + delimiter)
def t_decimal_number(t):
    'a'
    pass

# print(t_nondecimal_number.__doc__)
# print(t_decimal_number.__doc__)


def t_ABBREV(t):
    # ' ` , ,@ #' #` #, #,@
    r"\#?(?:'|`|,@|,)"
    t.value = {
        "'": 'quote',
        "`": 'quasiquote',
        ",": 'unquote',
        ",@": 'unquote-splicing',
        "#'": 'syntax',
        "#`": 'quasisyntax',
        "#,": 'unsyntax',
        "#,@": 'unsyntax-splicing',
        }[t.value]
    return t

def t_error(t):
    raise MySyntaxError(t)

# scanner = lex.lex()

grammar = r'''
<lexeme> -> <identifier> | <boolean> | <number>
          | <character> | <string>
          | ( | ) | [ | ] | #( | #vu8( | ' | ` | , | ,@ | .
          | #' | #` | #, | #,@
<delimiter> -> ( | ) | [ | ] | " | ; | #
          | <whitespace>
<whitespace> -> <character tabulation>
         | <linefeed> | <line tabulation> | <form feed>
         | <carriage return> | <next line>
         | <any character whose category is Zs, Zl, or Zp>
<line ending> -> <linefeed> | <carriage return>
         | <carriage return> <linefeed> | <next line>
         | <carriage return> <next line> | <line separator>
<comment> -> ;  <all subsequent characters up to a
                             <line ending> or <paragraph separator>>
         | <nested comment>
         | #; <interlexeme space> <datum>
         | #!r6rs
<nested comment> -> #|  <comment text>
         <comment cont>* |#
<comment text> ->  <character sequence not containing #| or |#>
<comment cont> -> <nested comment> <comment text>
<atmosphere> -> <whitespace> | <comment>
<interlexeme space> -> <atmosphere>*





<identifier> -> <initial> <subsequent>*
           | <peculiar identifier>
<initial> -> <constituent> | <special initial>
          | <inline hex escape>
<letter> ->  a | b | c | ... | z
         | A | B | C | ... | Z
<constituent> -> <letter>
          | <any character whose Unicode scalar value is greater than
              127, and whose category is Lu, Ll, Lt, Lm, Lo, Mn,
              Nl, No, Pd, Pc, Po, Sc, Sm, Sk, So, or Co>
<special initial> -> ! | $ | % | & | * | / | : | < | =
           | > | ? | ^ | _ | ~
<subsequent> -> <initial> | <digit>
           | <any character whose category is Nd, Mc, or Me>
           | <special subsequent>
<digit> -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
<hex digit> -> <digit>
          | a | A | b | B | c | C | d | D | e | E | f | F
<special subsequent> -> + | - | . | @
<inline hex escape> -> \x<hex scalar value>;
<hex scalar value> -> <hex digit>+
<peculiar identifier> -> + | - | ... | -> <subsequent>*
<boolean> -> #t | #T | #f | #F
<character> -> #\<any character>
           | #\<character name>
           | #\x<hex scalar value>
<character name> -> nul | alarm | backspace | tab
         | linefeed | newline | vtab | page | return
         | esc | space | delete
<string> -> " <string element>* "
<string element> -> <any character other than " or \>
          | \a | \b | \t | \n | \v | \f | \r
           | \" | \\
           | \<intraline whitespace><line ending>
              <intraline whitespace>
           | <inline hex escape>
<intraline whitespace> -> <character tabulation>
         | <any character whose category is Zs>




<number> -> <num 2> | <num 8>
             | <num 10> | <num 16>
<num R> -> <prefix R> <complex R>
<complex R> -> <real R> | <real R> @ <real R>
            | <real R> + <ureal R> i | <real R> - <ureal R> i
            | <real R> + <naninf> i | <real R> - <naninf> i
            | <real R> + i | <real R> - i
            | + <ureal R> i | - <ureal R> i 
            | + <naninf> i | - <naninf> i
            | + i | - i
<real R> -> <sign> <ureal R>
           | + <naninf> | - <naninf>
<naninf> -> nan.0 | inf.0
<ureal R> -> <uinteger R>
            | <uinteger R> / <uinteger R>
            | <decimal R> <mantissa width>
<decimal 10> -> <uinteger 10> <suffix>
            | . <digit 10>+ <suffix>
            | <digit 10>+ . <digit 10>* <suffix>
            | <digit 10>+ . <suffix>
<uinteger R> -> <digit R>+
<prefix R> -> <radix R> <exactness>
            | <exactness> <radix R>


<suffix> -> <empty> 
            | <exponent marker> <sign> <digit 10>+
<exponent marker> -> e | E | s | S | f | F
            | d | D | l | L
<mantissa width> -> <empty>
            | | <digit 10>+
<sign> -> <empty>  | + |  -
<exactness> -> <empty>
            | #i| #I | #e | #E
<radix 2> -> #b | #B
<radix 8> -> #o | #O
<radix 10> -> <empty> | #d | #D
<radix 16> -> #x | #X
<digit 2> -> 0 | 1
<digit 8> -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7
<digit 10> -> <digit>
<digit 16> -> <hex digit>
'''

# Datum syntax
'''
<datum> -> <lexeme datum>
          | <compound datum>
<lexeme datum> -> <boolean> | <number>
          | <character> | <string> |  <symbol>
<symbol> -> <identifier>
<compound datum> -> <list> | <vector> | <bytevector>
<list> -> (<datum>*) | [<datum>*]
            | (<datum>+ . <datum>) | [<datum>+ . <datum>]
            | <abbreviation>
<abbreviation> -> <abbrev prefix> <datum>
<abbrev prefix> -> ' | ` | , | ,@
            | #' | #` | #, | #,@
<vector> -> #(<datum>*)
<bytevector> -> #vu8(<u8>*)
<u8> -> <any <number> representing an exact
                    integer in {0, ..., 255}>
'''
