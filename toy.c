#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#include "unicode.h"

#include "toy_data.h"

int char_class(int c)
{
    if (c < 128)
	return toy_charmap[c];
    /* XXX handle line_separator and next_line */
    else
      return toy_unicode_catmap[unicode_general_category(c)];
   return -1;
}

int is_delimiter(int c)
{
    switch (c) {

    case ' ':
    case '\t':
    case '\n':
    case ';':
    case '(':
    case ')':
	return 1;

    default:
	return 0;
    }
}

int token_needs_delimiter(toy_token_t t)
{
    switch (t) {

    case TOY_ONE:
    case TOY_TWO:
	return 1;

    default:
	return 0;
    }
}

const char *token_name(toy_token_t t)
{
    switch (t) {
    case TOY_ATMOSPHERE:
	return "atmosphere";
    case TOY_ONE:
	return "ab";
    case TOY_TWO:
	return "[a-c][bde]";
    case TOY_OPEN:
	return "open";
    case TOY_CLOSE:
	return "close";
    default:
	return "???";
    }
}

toy_state_t scan_char(toy_state_t state, int c)
{
    toy_state_t next_state;
    if (state < TOY_ACCEPT_COUNT) {
	toy_token_t tok = toy_accepts[state];
	if (token_needs_delimiter(tok) && is_delimiter(c)) {
	    printf("A token %s\n", token_name(tok));
	    state = TOY_INITIAL_STATE;
	    return state;
	}
    }
    toy_cc_t cc = char_class(c);
    const toy_delta_row_t *row = &toy_delta[state];
    if (cc < row->toy_len)
	next_state = toy_next_states[row->toy_index + cc];
    else
	next_state = TOY_COMMON_STATE;
    if (next_state < TOY_ACCEPT_COUNT) {
	toy_token_t tok = toy_accepts[next_state];
	if (tok == TOY_ATMOSPHERE) {
	    printf("atmosphere\n");
	    next_state = TOY_INITIAL_STATE;
	}
	else if (!token_needs_delimiter(tok)) {
	    printf("B token %s\n", token_name(tok));
	    next_state = TOY_INITIAL_STATE;
	}
    } else if (next_state == TOY_ERROR_STATE) {
	printf("error\n");
	next_state = TOY_INITIAL_STATE;
    }
    // printf("'\\%03o' cc %d state %d -> %d\n", c, cc, state, next_state);
    return next_state;
}

int main()
{
    toy_state_t state = TOY_INITIAL_STATE;
    int c;
    while ((c = getchar()) != EOF) {
	state = scan_char(state, c);
    }
    return 0;
}
