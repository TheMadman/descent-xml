#include <libadt/str.h>
#include <xmltree/parse.h>

typedef struct xmltree_lex lex_t;

#define str libadt_str_literal

#define lex xmltree_lex_init
#define eof xmltree_classifier_eof
#define unexpected xmltree_classifier_unexpected
#define error xmltree_parse_error
#define parse xmltree_parse_cstr

static int is_end_type(lex_t token)
{
	return token.type == eof;
}

static int is_error_type(lex_t token)
{
	return token.type == unexpected
		|| token.type == error;
}

int main()
{
	lex_t token = lex(str("<element>Hello, world!</element>"));
	while (!is_end_type(token)) {
		if (is_error_type(token)) {
			// Handle error
			return 1;
		}

		token = parse(token, NULL, NULL, NULL);
	}
}
