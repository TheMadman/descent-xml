#include "descent_xml/lex.h"

size_t _descent_xml_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
bool _descent_xml_read_error(_descent_xml_read_t read);
_descent_xml_read_t _descent_xml_read(
	struct libadt_const_lptr script,
	descent_xml_classifier_fn *const previous
);
struct descent_xml_lex descent_xml_lex_init(
	struct libadt_const_lptr script
);
struct descent_xml_lex descent_xml_lex_next_raw(
	struct descent_xml_lex previous
);
