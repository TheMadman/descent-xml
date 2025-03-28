#include "xmltree/lex.h"

inline size_t _xmltree_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);

inline bool _xmltree_read_error(_xmltree_read_t read);
inline _xmltree_read_t _xmltree_read(
	struct libadt_const_lptr script,
	xmltree_classifier_fn *const previous
);
XMLTREE_EXPORT inline struct xmltree_lex xmltree_lex_init(
	struct libadt_const_lptr script
);
