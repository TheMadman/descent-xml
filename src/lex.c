#include "xmltree/lex.h"

size_t _xmltree_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
bool _xmltree_read_error(_xmltree_read_t read);
_xmltree_read_t _xmltree_read(
	struct libadt_const_lptr script,
	xmltree_classifier_fn *const previous
);
struct xmltree_lex xmltree_lex_init(
	struct libadt_const_lptr script
);
struct xmltree_lex xmltree_lex_next_raw(
	struct xmltree_lex previous
);
