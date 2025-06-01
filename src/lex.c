#include "descent_xml/lex.h"

#include "descent_xml/classifier.h"

#include <wchar.h>
#include <wctype.h>

typedef descent_xml_classifier_void_fn vfn;

ssize_t _descent_xml_lex_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
bool _descent_xml_lex_read_error(_descent_xml_lex_read_t read);
_descent_xml_lex_read_t _descent_xml_lex_read(
	struct libadt_const_lptr script,
	descent_xml_classifier_fn *const previous
);
struct descent_xml_lex descent_xml_lex_init(
	struct libadt_const_lptr script
);
struct descent_xml_lex descent_xml_lex_next_raw(
	struct descent_xml_lex previous
);
int _descent_xml_lex_startswith(
	struct libadt_const_lptr string,
	struct libadt_const_lptr start
);
struct descent_xml_lex _descent_xml_lex_handle_doctype(
	struct descent_xml_lex token
);
ssize_t _descent_xml_lex_count_spaces(
	struct libadt_const_lptr next
);
struct libadt_const_lptr _descent_xml_lex_remainder(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_then(
	struct descent_xml_lex token,
	_descent_xml_lex_section *section
);
struct descent_xml_lex _descent_xml_lex_else(
	struct descent_xml_lex token,
	_descent_xml_lex_section *section
);
struct descent_xml_lex _descent_xml_lex_optional(
	struct descent_xml_lex token,
	_descent_xml_lex_section *section
);
struct descent_xml_lex _descent_xml_lex_space(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_name(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_quote_string(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_doctype_str(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_doctype_system(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_doctype_public(
	struct descent_xml_lex token
);
struct descent_xml_lex _descent_xml_lex_doctype_extrawurst(
	struct descent_xml_lex token
);

// These functions are special, in that they're not
// really doing all the processing, that's up to lex_next_raw
vfn *descent_xml_lex_doctype(wchar_t input)
{
	switch (input) {
		case L'>':
			return (vfn*)descent_xml_classifier_element_end;
		default:
			return (vfn*)descent_xml_classifier_unexpected;
	}
}

vfn *descent_xml_lex_cdata(wchar_t input)
{
	switch (input) {
		case L'\0':
			return (vfn*)descent_xml_classifier_unexpected;
		case ']':
			return (vfn*)descent_xml_lex_cdata_end;
		default:
			return (vfn*)descent_xml_lex_cdata_text;
	}
}

vfn *descent_xml_lex_cdata_text(wchar_t input)
{
	return descent_xml_lex_cdata(input);
}

vfn *descent_xml_lex_cdata_end(wchar_t input)
{
	return descent_xml_classifier_element_end(input);
}

