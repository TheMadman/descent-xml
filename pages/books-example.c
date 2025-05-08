#include <libadt/str.h>
#include <descent_xml/parse.h>

#include <stdio.h>

typedef struct descent_xml_lex lex_t;

#define str libadt_str_literal

#define lex descent_xml_lex_init
#define eof descent_xml_classifier_eof
#define unexpected descent_xml_classifier_unexpected
#define close descent_xml_classifier_element_close
#define error descent_xml_parse_error
#define parse descent_xml_parse_cstr

#define XML \
"<?xml version=\"1.0\" ?>\n" \
"<library>\n" \
"	<book type=\"non-fiction\">\n" \
"		<title>The Pragmatic Programmer</title>\n" \
"		<author>David Thomas</author>\n" \
"		<author>Andrew Hunt</author>\n" \
"	</book>\n" \
"	<book type=\"fiction\">\n" \
"		<title>Magician</title>\n" \
"		<author>Raymond E. Feist</author>\n" \
"	</book>\n" \
"	<book type=\"non-fiction\">\n" \
"		<title>Operating Systems Principles &amp; Practice Second Edition</title>\n" \
"		<author>Thomas Anderson</author>\n" \
"		<author>Michael Dahlin</author>\n" \
"	</book>\n" \
"	<book type=\"fiction\">\n" \
"		<title>Stormbreaker</title>\n" \
"		<author>Anthony Horowitz</author>\n" \
"	</book>\n" \
"</library>"

static int is_end_type(lex_t token)
{
	return token.type == eof;
}

static int is_error_type(lex_t token)
{
	return token.type == unexpected
		|| token.type == error;
}

static int equal(const char *a, const char *b)
{
	return strcmp(a, b) == 0;
}

lex_t text_printer(
	lex_t token,
	char *text,
	void *context
)
{
	(void)context;
	printf("%s", text);
	return token;
}

lex_t author_handler(
	lex_t token,
	char *element_name,
	char **attributes,
	bool empty,
	void *context
)
{
	(void)context;
	(void)attributes;
	if (!empty && equal(element_name, "author")) {
		printf("Author: ");
		while (token.type != close)
			token = parse(token, NULL, text_printer, NULL);
		printf("\n");
	} else if (!empty) {
		while (token.type != close)
			token = parse(token, NULL, NULL, NULL);
	}
	token = parse(token, NULL, NULL, NULL);
	return token;
}

lex_t book_handler(
	lex_t token,
	char *element_name,
	char **attributes,
	bool empty,
	void *context
)
{
	(void)context;
	if (empty || !equal(element_name, "book"))
		return token;

	for (; *attributes; attributes += 2) {
		if (
			equal(attributes[0], "type")
			&& equal(attributes[1], "fiction")
		) {
			while (token.type != close) {
				token = parse(token, author_handler, NULL, NULL);
			}
			token = parse(token, NULL, NULL, NULL);
		}
	}
	return token;
}

int main()
{
	lex_t token = lex(str(XML));
	while (!is_end_type(token)) {
		if (is_error_type(token)) {
			// Handle error
			return 1;
		}

		token = parse(token, book_handler, NULL, NULL);
	}
}
