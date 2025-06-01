/*
 * XMLTree - An XML Parser-Helper Library
 * Copyright (C) 2025  Marcus Harrison
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DESCENT_XML_LEX
#define DESCENT_XML_LEX

#ifdef __cplusplus
extern "C" {
#endif



#include <wchar.h>
#include <wctype.h>

#include <libadt.h>

#include "classifier.h"

/**
 * \file
 */

/**
 * \brief Represents a single token.
 */
struct descent_xml_lex {
	/**
	 * \brief Represents the type of token classifiered.
	 */
	descent_xml_classifier_fn *type;

	/**
	 * \brief A pointer to the full script.
	 */
	struct libadt_const_lptr script;

	/**
	 * \brief A pointer to the classifiered value.
	 *
	 * This will always be a pointer into .script.
	 */
	struct libadt_const_lptr value;
};

inline ssize_t _descent_xml_lex_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
)
{
	if (string.length <= 0) {
		*result = L'\0';
		return 0;
	}
	return (ssize_t)mbrtowc(
		result,
		string.buffer,
		(size_t)string.length,
		_mbstate
	);
}

typedef struct {
	ssize_t amount;
	descent_xml_classifier_fn *type;
	struct libadt_const_lptr script;
} _descent_xml_lex_read_t;

inline bool _descent_xml_lex_read_error(_descent_xml_lex_read_t read)
{
	return read.amount < 0
		|| read.type == descent_xml_classifier_unexpected;
}

inline _descent_xml_lex_read_t _descent_xml_lex_read(
	struct libadt_const_lptr script,
	descent_xml_classifier_fn *const previous
)
{
	wchar_t c = 0;
	mbstate_t mbs = { 0 };
	_descent_xml_lex_read_t result = { 0 };
	result.amount = _descent_xml_lex_mbrtowc(&c, script, &mbs);
	if (_descent_xml_lex_read_error(result))
		result.type = (descent_xml_classifier_fn*)descent_xml_classifier_unexpected;
	else
		result.type = (descent_xml_classifier_fn*)previous(c);

	result.script = libadt_const_lptr_index(script, (ssize_t)result.amount);
	return result;
}

descent_xml_classifier_void_fn *descent_xml_lex_doctype(wchar_t input);
descent_xml_classifier_void_fn *descent_xml_lex_cdata(wchar_t input);
descent_xml_classifier_void_fn *descent_xml_lex_cdata_text(wchar_t input);
descent_xml_classifier_void_fn *descent_xml_lex_cdata_end(wchar_t input);

/**
 * \brief Initializes a token object for use in descent_xml_lex_next().
 *
 * \param script The script to create a token from.
 *
 * \returns A token, valid for passing to descent_xml_lex_next().
 */
inline struct descent_xml_lex descent_xml_lex_init(
	struct libadt_const_lptr script
)
{
	return (struct descent_xml_lex) {
		.type = (descent_xml_classifier_fn*)descent_xml_classifier_start,
		.script = script,
		.value = libadt_const_lptr_truncate(script, 0),
	};
}

inline int _descent_xml_lex_startswith(
	struct libadt_const_lptr string,
	struct libadt_const_lptr start
)
{
	if (string.size != start.size)
		return 0;
	if (string.length < start.length)
		return 0;

	return libadt_const_lptr_equal(
		libadt_const_lptr_truncate(string, (size_t)start.length),
		start
	);
}

inline ssize_t _descent_xml_lex_count_spaces(
	struct libadt_const_lptr next
)
{
	ssize_t spaces = 0;
	wchar_t c = 0;
	mbstate_t mbstate = { 0 };
	for (
		ssize_t current = _descent_xml_lex_mbrtowc(&c, next, &mbstate);
		iswspace((wint_t)c);
		next = libadt_const_lptr_index(next, current),
		current = _descent_xml_lex_mbrtowc(&c, next, &mbstate)
	) {
		const bool unexpected = c == L'\0'
			|| current < 0;
		if (unexpected)
			return -1;

		spaces += current;
	}

	return spaces;
}

inline struct libadt_const_lptr _descent_xml_lex_remainder(
	struct descent_xml_lex token
)
{
	return libadt_const_lptr_after(token.script, token.value);
}

typedef struct descent_xml_lex _descent_xml_lex_section(struct descent_xml_lex);

inline struct descent_xml_lex _descent_xml_lex_then(
	struct descent_xml_lex token,
	_descent_xml_lex_section *section
)
{
	if (token.type == descent_xml_classifier_unexpected)
		return token;
	return section(token);
}

inline struct descent_xml_lex _descent_xml_lex_else(
	struct descent_xml_lex token,
	_descent_xml_lex_section *section
)
{
	if (token.type == descent_xml_classifier_unexpected)
		return section(token);
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_optional(
	struct descent_xml_lex token,
	_descent_xml_lex_section *section
)
{
	struct descent_xml_lex result
		= _descent_xml_lex_then(token, section);
	if (result.type == descent_xml_classifier_unexpected)
		return token;
	return result;
}

inline struct descent_xml_lex _descent_xml_lex_space(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr remainder
		= _descent_xml_lex_remainder(token);
	ssize_t spaces = _descent_xml_lex_count_spaces(remainder);
	if (spaces <= 0) {
		token.type = descent_xml_classifier_unexpected;
		return token;
	}
	token.value.length += spaces;
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_name(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr remainder
		= _descent_xml_lex_remainder(token);
	_descent_xml_lex_read_t read
		= _descent_xml_lex_read(remainder, descent_xml_classifier_element);

	if (read.type == descent_xml_classifier_unexpected) {
		token.type = read.type;
		return token;
	}

	ssize_t total = 0;
	while (read.type == descent_xml_classifier_element_name) {
		if (_descent_xml_lex_read_error(read)) {
			token.type = descent_xml_classifier_unexpected;
			return token;
		}

		total += read.amount;
		read = _descent_xml_lex_read(read.script, read.type);
	}

	token.value.length += total;
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_quote_string(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr remainder
		= _descent_xml_lex_remainder(token);
	_descent_xml_lex_read_t read
		= _descent_xml_lex_read(
			remainder,
			descent_xml_classifier_attribute_assign
		);

	// these names are too fucking long
	if (
		_descent_xml_lex_read_error(read)
		|| read.type != descent_xml_classifier_attribute_value_single_quote_start
		|| read.type != descent_xml_classifier_attribute_value_double_quote_start
	) {
		token.type = descent_xml_classifier_unexpected;
		return token;
	}

	ssize_t total = 0;

	while (
		read.type != descent_xml_classifier_attribute_value_single_quote_end
		|| read.type != descent_xml_classifier_attribute_value_double_quote_end
	) {
		if (_descent_xml_lex_read_error(read)) {
			token.type = read.type;
			return token;
		}

		total += read.amount;
		read = _descent_xml_lex_read(read.script, read.type);
	}

	token.value.length += total;
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_doctype_str(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr remainder
		= _descent_xml_lex_remainder(token);
	const struct libadt_const_lptr
		doctypedecl = libadt_str_literal("!DOCTYPE");

	if (!_descent_xml_lex_startswith(remainder, doctypedecl)) {
		token.type = descent_xml_classifier_unexpected;
		return token;
	}

	token.value.length += doctypedecl.length;
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_doctype_system(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr remainder
		= _descent_xml_lex_remainder(token);
	const struct libadt_const_lptr
		systemid = libadt_str_literal("SYSTEM");

	if (!_descent_xml_lex_startswith(remainder, systemid)) {
		token.type = descent_xml_classifier_unexpected;
		return token;
	}

	token.value.length += systemid.length;
	token = _descent_xml_lex_then(token, _descent_xml_lex_space);
	token = _descent_xml_lex_then(token, _descent_xml_lex_quote_string);
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_doctype_public(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr remainder
		= _descent_xml_lex_remainder(token);
	const struct libadt_const_lptr
		publicid = libadt_str_literal("PUBLIC");

	if (!_descent_xml_lex_startswith(remainder, publicid)) {
		token.type = descent_xml_classifier_unexpected;
		return token;
	}

	token.value.length += publicid.length;
	token = _descent_xml_lex_then(token, _descent_xml_lex_space);
	token = _descent_xml_lex_then(token, _descent_xml_lex_quote_string);
	token = _descent_xml_lex_then(token, _descent_xml_lex_space);
	token = _descent_xml_lex_then(token, _descent_xml_lex_quote_string);
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_doctype_extrawurst(
	struct descent_xml_lex token
)
{
	token = _descent_xml_lex_then(
		token,
		_descent_xml_lex_space
	);
	if (token.type == descent_xml_classifier_unexpected)
		return token;

	token = _descent_xml_lex_then(
		token,
		_descent_xml_lex_doctype_system
	);
	token = _descent_xml_lex_else(
		token,
		_descent_xml_lex_doctype_public
	);
	return token;
}

inline struct descent_xml_lex _descent_xml_lex_handle_doctype(
	struct descent_xml_lex token
)
{
	token = _descent_xml_lex_then(token, _descent_xml_lex_doctype_str);
	token = _descent_xml_lex_then(token, _descent_xml_lex_space);
	token = _descent_xml_lex_then(token, _descent_xml_lex_name);
	token = _descent_xml_lex_optional(
		token,
		_descent_xml_lex_doctype_extrawurst
	);
	token = _descent_xml_lex_optional(
		token,
		_descent_xml_lex_space
	);

	if (token.type != descent_xml_classifier_unexpected) {
		token.value = libadt_const_lptr_index(token.value, 1);
		token.type = descent_xml_lex_doctype;
	}
	return token;
}

/**
 * \brief Returns the next, raw token in the script referred to by
 * 	previous.
 *
 * \param previous The previous token from the script.
 *
 * \returns The next token.
 */
inline struct descent_xml_lex descent_xml_lex_next_raw(
	struct descent_xml_lex token
)
{
	const struct libadt_const_lptr
		doctypedecl = libadt_str_literal("!DOCTYPE"),
		cdata = libadt_str_literal("![CDATA[");
	struct libadt_const_lptr next = _descent_xml_lex_remainder(token);

	if (
		token.type == descent_xml_classifier_element
		&& _descent_xml_lex_startswith(next, doctypedecl)
	)
		return _descent_xml_lex_handle_doctype(token);

	_descent_xml_lex_read_t
		read = _descent_xml_lex_read(next, token.type),
		previous_read = read;

	if (_descent_xml_lex_read_error(read))
		return (struct descent_xml_lex) {
			.script = token.script,
			.type = descent_xml_classifier_unexpected,
			.value = libadt_const_lptr_truncate(next, 0),
		};

	if (read.type == descent_xml_classifier_eof) {
		return (struct descent_xml_lex) {
			.script = token.script,
			.type = read.type,
			.value = libadt_const_lptr_truncate(next, (size_t)read.amount)
		};
	}

	ssize_t value_length = read.amount;
	for (
		read = _descent_xml_lex_read(read.script, read.type);
		!_descent_xml_lex_read_error(read);
		read = _descent_xml_lex_read(read.script, read.type)
	) {
		if (read.type != previous_read.type)
			break;

		previous_read = read;
		value_length += read.amount;
	}

	return (struct descent_xml_lex) {
		.script = token.script,
		.type = previous_read.type,
		.value = libadt_const_lptr_truncate(next, (size_t)value_length),
	};
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DESCENT_XML_LEX
