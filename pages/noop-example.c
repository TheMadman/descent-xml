#include <libadt/str.h>
#include <descent-xml.h>

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

int main()
{
	lex_t token = lex(str("<element>Hello, world!</element>"));
	if (!valid(token))
		return 1;

	while (!is_end_type(token)) {
		if (is_error_type(token)) {
			// Handle error
			return 1;
		}

		token = parse(token, NULL, NULL, NULL);
	}
}
