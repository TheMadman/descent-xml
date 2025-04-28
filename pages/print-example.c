#include <libadt/str.h>
#include <xmltree/parse.h>

#include <stdio.h>

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

lex_t element_handler(
	lex_t token,
	char *element_name,
	char **attributes,
	bool empty,
	void *context
)
{
	(void)context;
	printf("element_name: %s\n", element_name);
	for (; *attributes; attributes += 2) {
		printf("attribute: %s=%s\n", attributes[0], attributes[1]);
	}
	printf("empty element (ends with /> or ?>): %s\n", empty? "true": "false");
	return token;
}

lex_t text_handler(
	lex_t token,
	char *text,
	void *context
)
{
	(void)context;
	printf("text node: %s\n", text);
	return token;
}

int main()
{
	lex_t token = lex(str(
		"<?xml version=\"1.0\"?>"
		"<element attr=\"val\" attr2=\"val2\">"
			"Hello, world!"
		"</element>"
	));
	while (!is_end_type(token)) {
		if (is_error_type(token)) {
			// Handle error
			return 1;
		}

		token = parse(token, element_handler, text_handler, NULL);
	}
}
