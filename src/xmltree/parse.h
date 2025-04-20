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

#ifndef XMLTREE_PARSE
#define XMLTREE_PARSE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "init.h"
#include "lex.h"

#include <libadt/lptr.h>
#include <libadt/vector.h>

/**
 * \file
 */

typedef struct xmltree_lex xmltree_parse_element_fn(
	struct xmltree_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);

typedef struct xmltree_lex xmltree_parse_text_fn(
	struct xmltree_lex token,
	struct libadt_const_lptr text,
	void *context
);

inline bool _xmltree_end_token(struct xmltree_lex token)
{
	return token.type == xmltree_classifier_eof
		|| token.type == xmltree_classifier_unexpected;
}

inline bool _xmltree_is_attribute_value_type(struct xmltree_lex token)
{
	return token.type == xmltree_classifier_attribute_value_single_quote
		|| token.type == xmltree_classifier_attribute_value_single_quote_entity_start
		|| token.type == xmltree_classifier_attribute_value_single_quote_entity
		|| token.type == xmltree_classifier_attribute_value_double_quote
		|| token.type == xmltree_classifier_attribute_value_double_quote_entity_start
		|| token.type == xmltree_classifier_attribute_value_double_quote_entity;

}

typedef struct {
	struct libadt_const_lptr value;
	struct xmltree_lex token;
} _xmltree_value_t;

inline _xmltree_value_t _xmltree_attribute_value(
	struct xmltree_lex token
)
{
	if (!_xmltree_is_attribute_value_type(token))
		return (_xmltree_value_t) {
			libadt_const_lptr_truncate(token.value, 0),
			token
		};

	struct libadt_const_lptr result = token.value;
	struct xmltree_lex next = xmltree_lex_next_raw(token);
	while (_xmltree_is_attribute_value_type(next)) {
		result.length += next.value.length;
		next = xmltree_lex_next_raw(next);
	}
	return (_xmltree_value_t) { result, next };
}

inline struct xmltree_lex _xmltree_handle_element(
	struct xmltree_lex token,
	xmltree_parse_element_fn *element_handler,
	void *context
)
{
	// not keen on how long this function is getting
	if (
		token.type == xmltree_classifier_unexpected
		|| token.type == xmltree_classifier_element_close
	)
		return token;
	const struct libadt_const_lptr name = token.value;

	token = xmltree_lex_next_raw(token);
	if (token.type == xmltree_classifier_unexpected)
		return token;

	LIBADT_VECTOR_WITH(attributes, sizeof(struct libadt_const_lptr), 0) {
		while (token.type == xmltree_classifier_element_space) {
			token = xmltree_lex_next_raw(token);

			if (token.type == xmltree_classifier_attribute_name) {
				attributes = libadt_vector_append(
					attributes,
					&token.value
				);
				token = xmltree_lex_next_raw(token);
				if (token.type == xmltree_classifier_attribute_expect_assign)
					token = xmltree_lex_next_raw(token);
				if (token.type == xmltree_classifier_attribute_assign)
					token = xmltree_lex_next_raw(token);
				const bool quote =
					token.type == xmltree_classifier_attribute_value_single_quote_start
					|| token.type == xmltree_classifier_attribute_value_double_quote_start;
				if (quote)
					token = xmltree_lex_next_raw(token);

				_xmltree_value_t attr
					= _xmltree_attribute_value(token);
				attributes = libadt_vector_append(
					attributes,
					&attr.value
				);
				token = attr.token;
				token = xmltree_lex_next_raw(token);
			}
		}

		if (token.type == xmltree_classifier_unexpected)
			continue;

		const bool is_empty
			= token.type == xmltree_classifier_element_empty;

		if (is_empty || token.type == xmltree_classifier_element_end) {
			struct libadt_const_lptr attribsptr = {
				.buffer = attributes.buffer,
				.size = sizeof(struct libadt_const_lptr),
				.length = (ssize_t)attributes.length,
			};

			token = element_handler(
				token,
				name,
				attribsptr,
				is_empty,
				context
			);
		}
	}
	return token;
}

inline bool _xmltree_is_text_type(struct xmltree_lex token)
{
	return token.type == xmltree_classifier_text
		|| token.type == xmltree_classifier_text_entity_start
		|| token.type == xmltree_classifier_text_entity;
}

inline _xmltree_value_t _xmltree_text_value(
	struct xmltree_lex token
)
{
	struct libadt_const_lptr result = token.value;
	struct xmltree_lex next = xmltree_lex_next_raw(token);
	while (_xmltree_is_text_type(next)) {
		result.length += next.value.length;
		next = xmltree_lex_next_raw(next);
	}
	return (_xmltree_value_t) { result, next };
}

inline struct xmltree_lex _xmltree_handle_text(
	struct xmltree_lex token,
	xmltree_parse_text_fn *text_handler,
	void *context
)
{
	_xmltree_value_t result = _xmltree_text_value(token);
	return text_handler(result.token, result.value, context);
}

XMLTREE_EXPORT inline struct xmltree_lex xmltree_parse(
	struct libadt_const_lptr xml,
	xmltree_parse_element_fn *element_handler,
	xmltree_parse_text_fn *text_handler,
	void *context
)
{
	struct xmltree_lex token = xmltree_lex_init(xml);

	while (!_xmltree_end_token(token)) {
		token = xmltree_lex_next_raw(token);

		if (token.type == xmltree_classifier_element && element_handler) {
			token = _xmltree_handle_element(
				xmltree_lex_next_raw(token),
				element_handler,
				context
			);
		} else if (_xmltree_is_text_type(token) && text_handler) {
			token = _xmltree_handle_text(
				token,
				text_handler,
				context
			);
		}
	}

	return token;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // XMLTREE_PARSE
