#include "descent_xml/validate.h"

struct descent_xml_lex _descent_xml_validate_element_handler(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);
bool descent_xml_validate_element_depth(struct descent_xml_lex token, int depth);
bool descent_xml_validate_element(struct descent_xml_lex token);
struct descent_xml_lex _descent_xml_validate_xmldecl(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);
struct descent_xml_lex _descent_xml_validate_doctype(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);
bool descent_xml_validate_document_depth(
	struct descent_xml_lex token,
	int depth
);
bool descent_xml_validate_document(struct descent_xml_lex token);
bool _descent_xml_non_space_text(struct descent_xml_lex token);
struct descent_xml_lex _descent_xml_validate_parse_prolog(
	struct descent_xml_lex token
);
