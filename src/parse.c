#include "xmltree/parse.h"

bool _xmltree_end_token(struct xmltree_lex token);
struct xmltree_lex _xmltree_handle_element(
	struct xmltree_lex token,
	xmltree_parse_element_fn *element_handler,
	void *context
);
struct xmltree_lex xmltree_parse(
	struct libadt_const_lptr xml,
	xmltree_parse_element_fn *element_handler,
	xmltree_parse_text_fn *text_handler,
	void *context
);
struct xmltree_lex _xmltree_handle_text(
	struct xmltree_lex token,
	xmltree_parse_text_fn *element_handler,
	void *context
);
_xmltree_value_t _xmltree_attribute_value(
	struct xmltree_lex token
);
bool _xmltree_is_attribute_value_type(
	struct xmltree_lex token
);
bool _xmltree_is_text_type(struct xmltree_lex token);
_xmltree_value_t _xmltree_text_value(struct xmltree_lex token);

