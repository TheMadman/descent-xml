#include "xmltree/validate.h"

struct xmltree_lex _xmltree_validate_element_handler(
	struct xmltree_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);
bool xmltree_validate_element(struct xmltree_lex token);
struct xmltree_lex _xmltree_validate_document(
	struct xmltree_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);
