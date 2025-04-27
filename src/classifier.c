#include "xmltree/classifier.h"

#include <wctype.h>
#include <stdbool.h>
#include <stdlib.h>

// https://www.w3.org/TR/REC-xml/#sec-documents

typedef xmltree_classifier_fn cfn;
typedef xmltree_classifier_void_fn vfn;

// unexpected and eof must be separate so the
// pointers point to different locations
static vfn *unexpected_impl(wchar_t c)
{
	(void)c;

	// There is no "correct" thing to do here,
	// the developer must check for unexpected and
	// EOF. If they call these functions, they're
	// not checking.
	//
	// If they're not checking for unexpected/EOF,
	// they're not checking for NULL.
	//
	// Returning NULL is _sometimes_ as bad as
	// calling abort(), and _sometimes_ is worse.
	abort();
	return (vfn*)unexpected_impl;
}

static vfn *eof_impl(wchar_t c)
{
	(void)c;
	abort();
	return (vfn*)eof_impl;
}

cfn *const xmltree_classifier_unexpected = unexpected_impl;
cfn *const xmltree_classifier_eof = eof_impl;

// character classes
typedef enum {
	CCLASS_UNKNOWN = -1,
	CCLASS_EOF = 0,
	CCLASS_NAME_START,
	CCLASS_NAME,
	CCLASS_SPACE,
	CCLASS_TEXT,
	CCLASS_EQUALS = '=',
	CCLASS_HASH = '#',
	CCLASS_OBRACKET = '<',
	CCLASS_CBRACKET = '>',
	CCLASS_DQUOTE = '"',
	CCLASS_SQUOTE = '\'',
	CCLASS_REF_START = '%',
	CCLASS_ENTITY_START = '&',
	CCLASS_ENTITY_END = ';',
	CCLASS_EMARK = '!',
	CCLASS_DASH = '-',
	CCLASS_QMARK = '?',
	CCLASS_SLASH = '/',
} CHARACTER_CLASS;

static bool in(const wchar_t c, const wchar_t *set)
{
	for (; *set; set++)
		if (c == *set)
			return true;
	return false;
}

static bool between(const wchar_t start, const wchar_t c, const wchar_t end)
{
	return start <= c
		&& c <= end;
}

static CHARACTER_CLASS get_cclass(wchar_t c)
{
	if (in(c, L"<>'\"&#=;!-%/?"))
		return c;

	const bool is_name_start
		= in(c, L":_")
		|| iswalpha((wint_t)c)
		// I don't know what these numbers mean,
		// but they're in the spec
		// [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
		|| between(0xC0, c, 0xD6)
		|| between(0xD8, c, 0xF6)
		|| between(0xF8, c, 0x2FF)
		|| between(0x370, c, 0x37D)
		|| between(0x37F, c, 0x1FFF)
		|| between(0x200C, c, 0x200D)
		|| between(0x2070, c, 0x218F)
		|| between(0x2C00, c, 0x2FEF)
		|| between(0x3001, c, 0xD7FF)
		|| between(0xF900, c, 0xFDCF)
		|| between(0xFDF0, c, 0xFFFD)
		|| between(0x10000, c, 0xEFFFF);

	if (is_name_start)
		return CCLASS_NAME_START;

	const bool is_name
		= c == '.'
		|| iswdigit((wint_t)c)
		// more magic numbers
		// #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
		|| c == 0xB7
		|| between(0x300, c, 0x36F)
		|| between(0x203F, c, 0x2040);

	if (is_name)
		return CCLASS_NAME;

	if (in(c, L" \t\r\n"))
		return CCLASS_SPACE;

	if (c == (wchar_t)WEOF || c == 0)
		return CCLASS_EOF;

	return CCLASS_TEXT;
}

static cfn *entity_start(wchar_t input, cfn *cont)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
		case CCLASS_HASH:
			return cont;
		default:
			return xmltree_classifier_unexpected;
	}
}

static cfn *entity_cont(wchar_t input, cfn *cont, cfn *end)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
		case CCLASS_NAME:
			return cont;
		case CCLASS_ENTITY_END:
			return end;
		default:
			return xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_start(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_OBRACKET:
			return (vfn *)xmltree_classifier_element;
		case CCLASS_SPACE:
			return (vfn *)xmltree_classifier_start;
		// Ironically, the XML spec states empty
		// files are not valid XML documents, so
		// no EOF handling here
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_EMARK:
			// do comments properly sometime?
		case CCLASS_QMARK:
			// this one too?
		case CCLASS_NAME_START:
			return (vfn*)xmltree_classifier_element_name;
		case CCLASS_SLASH:
			return (vfn*)xmltree_classifier_element_close;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_empty(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_end(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_EOF:
			return (vfn*)xmltree_classifier_eof;
		case CCLASS_OBRACKET:
			return (vfn*)xmltree_classifier_element;
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_unexpected;
		case CCLASS_REF_START:
		case CCLASS_ENTITY_START:
			return (vfn*)xmltree_classifier_text_entity_start;
		default:
			return (vfn*)xmltree_classifier_text;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_close(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
			return (vfn*)xmltree_classifier_element_close_name;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_close_name(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
		case CCLASS_NAME:
			return (vfn*)xmltree_classifier_element_close_name;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_element_close_space;
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_close_space(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_element_close_space;
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_name(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
		case CCLASS_NAME:
		case CCLASS_DASH:
			return (vfn*)xmltree_classifier_element_name;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_element_space;
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		case CCLASS_SLASH:
		case CCLASS_QMARK:
			return (vfn*)xmltree_classifier_element_empty;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_element_space(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
			return (vfn*)xmltree_classifier_attribute_name;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_element_space;
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		case CCLASS_SLASH:
		case CCLASS_QMARK:
			return (vfn*)xmltree_classifier_element_empty;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_name(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_NAME_START:
		case CCLASS_NAME:
			return (vfn*)xmltree_classifier_attribute_name;
		case CCLASS_EQUALS:
			return (vfn*)xmltree_classifier_attribute_assign;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_attribute_expect_assign;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_expect_assign(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_EQUALS:
			return (vfn*)xmltree_classifier_attribute_assign;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_attribute_expect_assign;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_assign(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_attribute_assign;
		case CCLASS_SQUOTE:
			return (vfn*)xmltree_classifier_attribute_value_single_quote_start;
		case CCLASS_DQUOTE:
			return (vfn*)xmltree_classifier_attribute_value_double_quote_start;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_single_quote_start(wchar_t input)
{
	// has all the same behaviour as a value, but is a
	// different state to differentiate the single quote "'"
	// from the value
	return xmltree_classifier_attribute_value_single_quote(input);
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_single_quote(wchar_t input)
{
	switch (get_cclass(input)) {
		// dunno why the spec says
		// '<' specifically, but it does
		case CCLASS_OBRACKET:
		case CCLASS_EOF:
			return (vfn*)xmltree_classifier_unexpected;
		case CCLASS_SQUOTE:
			return (vfn*)xmltree_classifier_attribute_value_single_quote_end;
		case CCLASS_ENTITY_START:
		case CCLASS_REF_START:
			return (vfn*)xmltree_classifier_attribute_value_single_quote_entity_start;
		default:
			return (vfn*)xmltree_classifier_attribute_value_single_quote;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_single_quote_entity_start(
	wchar_t input
)
{
	return (vfn*)entity_start(
		input,
		xmltree_classifier_attribute_value_single_quote_entity
	);
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_single_quote_entity(
	wchar_t input
)
{
	return (vfn*)entity_cont(
		input,
		xmltree_classifier_attribute_value_single_quote_entity,
		xmltree_classifier_attribute_value_single_quote
	);
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_single_quote_end(
	wchar_t input
)
{
	switch (get_cclass(input)) {
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_element_space;
		case CCLASS_SLASH:
		case CCLASS_QMARK:
			return (vfn*)xmltree_classifier_element_empty;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_double_quote_start(
	wchar_t input
)
{
	return xmltree_classifier_attribute_value_double_quote(input);
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_double_quote(
	wchar_t input
)
{
	switch (get_cclass(input)) {
		case CCLASS_OBRACKET:
		case CCLASS_EOF:
			return (vfn*)xmltree_classifier_unexpected;
		case CCLASS_DQUOTE:
			return (vfn*)xmltree_classifier_attribute_value_double_quote_end;
		case CCLASS_ENTITY_START:
		case CCLASS_REF_START:
			return (vfn*)xmltree_classifier_attribute_value_double_quote_entity_start;
		default:
			return (vfn*)xmltree_classifier_attribute_value_double_quote;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_double_quote_entity_start(
	wchar_t input
)
{
	return (vfn*)entity_start(
		input,
		xmltree_classifier_attribute_value_double_quote_entity
	);
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_double_quote_entity(
	wchar_t input
)
{
	return (vfn*)entity_cont(
		input,
		xmltree_classifier_attribute_value_double_quote_entity,
		xmltree_classifier_attribute_value_double_quote
	);
}

XMLTREE_EXPORT vfn *xmltree_classifier_attribute_value_double_quote_end(
	wchar_t input
)
{
	switch (get_cclass(input)) {
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_element_end;
		case CCLASS_SPACE:
			return (vfn*)xmltree_classifier_element_space;
		case CCLASS_SLASH:
		case CCLASS_QMARK:
			return (vfn*)xmltree_classifier_element_empty;
		default:
			return (vfn*)xmltree_classifier_unexpected;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_text(wchar_t input)
{
	switch (get_cclass(input)) {
		case CCLASS_OBRACKET:
			return (vfn*)xmltree_classifier_element;
		case CCLASS_ENTITY_START:
		case CCLASS_REF_START:
			return (vfn*)xmltree_classifier_text_entity_start;
		case CCLASS_EOF:
			return (vfn*)xmltree_classifier_eof;
		case CCLASS_CBRACKET:
			return (vfn*)xmltree_classifier_unexpected;
		default:
			return (vfn*)xmltree_classifier_text;
	}
}

XMLTREE_EXPORT vfn *xmltree_classifier_text_entity_start(wchar_t input)
{
	return (vfn*)entity_start(
		input,
		xmltree_classifier_text_entity
	);
}

XMLTREE_EXPORT vfn *xmltree_classifier_text_entity(wchar_t input)
{
	return (vfn*)entity_cont(
		input,
		xmltree_classifier_text_entity,
		xmltree_classifier_text
	);
}

