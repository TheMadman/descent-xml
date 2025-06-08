#include <libadt/str.h>
#include <descent-xml.h>

#include <stdio.h>

typedef struct descent_xml_lex lex_t;

#define str libadt_str_literal

#define lex descent_xml_lex_init
#define eof descent_xml_classifier_eof
#define unexpected descent_xml_classifier_unexpected
#define error descent_xml_parse_error
#define parse descent_xml_parse_cstr
#define valid descent_xml_validate_document

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

void text_handler(
	char *text,
	bool is_cdata,
	void *context
)
{
	(void)context;
	(void)is_cdata;
	printf("text node: %s\n", text);
}

int main()
{
	lex_t token = lex(str(
		"<?xml version=\"1.0\"?>\n"
		"<element attr=\"val\" attr2=\"\">\n"
		"	Hello, world!\n"
		"	<![CDATA[Hello, CDATA!]]>\n"
		"	Hello, after CDATA!\n"
		"</element>"
	));
	if (!valid(token))
		return 1;

	while (!is_end_type(token)) {
		if (is_error_type(token)) {
			// Handle error
			return 1;
		}

		token = parse(token, element_handler, text_handler, NULL);
	}
}
