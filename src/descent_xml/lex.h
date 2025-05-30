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

#include <libadt/lptr.h>

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

inline size_t _descent_xml_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
)
{
	if (string.length <= 0) {
		// when does this break?
		*result = (wchar_t)WEOF;
		return 0;
	}
	return mbrtowc(
		result,
		string.buffer,
		(size_t)string.length,
		_mbstate
	);
}

typedef struct {
	size_t amount;
	descent_xml_classifier_fn *type;
	struct libadt_const_lptr script;
} _descent_xml_read_t;

inline bool _descent_xml_read_error(_descent_xml_read_t read)
{
	return read.amount == (size_t)-1
		|| read.amount == (size_t)-2
		|| read.type == descent_xml_classifier_unexpected;
}

inline _descent_xml_read_t _descent_xml_read(
	struct libadt_const_lptr script,
	descent_xml_classifier_fn *const previous
)
{
	wchar_t c = 0;
	mbstate_t mbs = { 0 };
	_descent_xml_read_t result = { 0 };
	result.amount = _descent_xml_mbrtowc(&c, script, &mbs);
	if (_descent_xml_read_error(result))
		result.type = (descent_xml_classifier_fn*)descent_xml_classifier_unexpected;
	else
		result.type = (descent_xml_classifier_fn*)previous(c);

	result.script = libadt_const_lptr_index(script, (ssize_t)result.amount);
	return result;
}

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

/**
 * \brief Returns the next, raw token in the script referred to by
 * 	previous.
 *
 * \param previous The previous token from the script.
 *
 * \returns The next token.
 */
inline struct descent_xml_lex descent_xml_lex_next_raw(
	struct descent_xml_lex previous
)
{
	const ssize_t value_offset = (char *)previous.value.buffer
		- (char *)previous.script.buffer;
	struct libadt_const_lptr next = libadt_const_lptr_index(
		previous.script,
		value_offset + previous.value.length
	);

	_descent_xml_read_t
		read = _descent_xml_read(next, previous.type),
		previous_read = read;

	if (_descent_xml_read_error(read))
		return (struct descent_xml_lex) {
			.script = previous.script,
			.type = descent_xml_classifier_unexpected,
			.value = libadt_const_lptr_truncate(next, 0),
		};

	if (read.type == descent_xml_classifier_eof) {
		return (struct descent_xml_lex) {
			.script = previous.script,
			.type = read.type,
			.value = libadt_const_lptr_truncate(next, read.amount)
		};
	}

	size_t value_length = read.amount;
	for (
		read = _descent_xml_read(read.script, read.type);
		!_descent_xml_read_error(read);
		read = _descent_xml_read(read.script, read.type)
	) {
		if (read.type != previous_read.type)
			break;

		previous_read = read;
		value_length += read.amount;
	}

	return (struct descent_xml_lex) {
		.script = previous.script,
		.type = previous_read.type,
		.value = libadt_const_lptr_truncate(next, value_length),
	};
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DESCENT_XML_LEX
