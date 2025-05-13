#include <libadt/str.h>
#include <descent_xml.h>

#include <stdio.h>

typedef struct descent_xml_lex lex_t;

#define str libadt_str_literal

#define lex descent_xml_lex_init
#define eof descent_xml_classifier_eof
#define unexpected descent_xml_classifier_unexpected
#define close descent_xml_classifier_element_close_name
#define error descent_xml_parse_error
#define parse descent_xml_parse_cstr
#define valid descent_xml_validate_document

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

		// Here, we check if the token we received
		// was an element closing name.
		//
		// This will stop this function from parsing
		// and pass parsing back up to the caller.
		while (token.type != close) {
			if (is_end_type(token) || is_error_type(token))
				return token;
			token = parse(token, NULL, text_printer, NULL);
		}
		printf("\n");
	} else if (!empty) {
		// We want this function to _always_ iterate
		// to the next closing element, so that the caller
		// doesn't end early on the closing tag of a child
		// element.
		while (token.type != close) {
			if (is_end_type(token) || is_error_type(token))
				return token;
			token = parse(token, NULL, NULL, NULL);
		}
	}
	// We iterate past the closing slash to allow the
	// caller to check for its own closing element.
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
			// This is why the author_handler had to
			// iterate once past its closing element.
			//
			// If author_handler didn't iterate once more,
			// the closing slash of the child element
			// would be assigned to token.
			//
			// The first child element's closing tag would
			// end this loop early.
			//
			// This is a pattern for processing nested
			// elements in this fashion.
			while (token.type != close) {
				if (is_end_type(token) || is_error_type(token))
					return token;
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
	if (!valid(token))
		return 1;

	while (!is_end_type(token)) {
		if (is_error_type(token)) {
			// Handle error
			return 1;
		}

		token = parse(token, book_handler, NULL, NULL);
	}
}
