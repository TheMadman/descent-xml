#include "descent_xml/parse.h"

#include <stdlib.h>

descent_xml_classifier_void_fn *descent_xml_parse_error(wchar_t c)
{
	(void)c;
	abort();
	return (descent_xml_classifier_void_fn *)descent_xml_parse_error;
}

bool _descent_xml_end_token(struct descent_xml_lex token);
struct descent_xml_lex _descent_xml_handle_element(
	struct descent_xml_lex token,
	descent_xml_parse_element_fn *element_handler,
	void *context
);
struct descent_xml_lex descent_xml_parse(
	struct descent_xml_lex xml,
	descent_xml_parse_element_fn *element_handler,
	descent_xml_parse_text_fn *text_handler,
	void *context
);
struct descent_xml_lex _descent_xml_handle_text(
	struct descent_xml_lex token,
	descent_xml_parse_text_fn *element_handler,
	void *context
);
_descent_xml_value_t _descent_xml_attribute_value(
	struct descent_xml_lex token
);
bool _descent_xml_is_attribute_value_type(
	struct descent_xml_lex token
);
bool _descent_xml_is_text_type(struct descent_xml_lex token);
_descent_xml_value_t _descent_xml_text_value(struct descent_xml_lex token);
struct descent_xml_lex _cstr_element_handler(
	struct descent_xml_lex xml,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);
struct descent_xml_lex _cstr_text_handler(
	struct descent_xml_lex xml,
	struct libadt_const_lptr text,
	void *context
);
struct descent_xml_lex descent_xml_parse_cstr(
	struct descent_xml_lex xml,
	descent_xml_parse_element_cstr_fn *element_handler,
	descent_xml_parse_text_cstr_fn *text_handler,
	void *context
);
