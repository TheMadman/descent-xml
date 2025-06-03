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

#ifndef DESCENT_XML_VALIDATE
#define DESCENT_XML_VALIDATE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <libadt/lptr.h>
#include <libadt/str.h>

#include "parse.h"

/**
 * \file
 */

inline struct descent_xml_lex _descent_xml_validate_element_handler(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context_p
)
{
	(void)attributes;
	struct {
		bool valid;
		int depth;
	} *context = context_p;
	if (context->depth == 0) {
		context->valid = false;
		return token;
	}
	context->depth--;

	const struct libadt_const_lptr xmldecl = libadt_str_literal("?xml");

	if (libadt_const_lptr_equal(element_name, xmldecl)) {
		context->valid = false;
		return token;
	}

	if (empty)
		return token;

	while (token.type != descent_xml_classifier_element_close_name) {
		token = descent_xml_parse(
			token,
			_descent_xml_validate_element_handler,
			NULL,
			context
		);
		if (
			token.type == descent_xml_classifier_unexpected
			|| token.type == descent_xml_classifier_eof
			|| !context->valid
		) {
			context->valid = false;
			return token;
		}
	}

	context->valid = libadt_const_lptr_equal(
		token.value,
		element_name
	);
	context->depth++;
	token = descent_xml_parse(token, NULL, NULL, NULL);
	return token;
}

inline bool descent_xml_validate_element_depth(
	struct descent_xml_lex token,
	int depth
)
{
	struct {
		bool valid;
		int depth;
	} context = { true, depth };
	while (token.type != descent_xml_classifier_element) {
		if (
			token.type == descent_xml_classifier_unexpected
			|| token.type == descent_xml_classifier_eof
		)
			return false;
		token = descent_xml_lex_next_raw(token);
	}
	token = descent_xml_parse(
		token,
		_descent_xml_validate_element_handler,
		NULL,
		&context
	);

	// We have to check for unexpected/eof here in case the
	// element handler never runs
	return
		context.valid
		&& token.type != descent_xml_classifier_unexpected
		&& token.type != descent_xml_classifier_eof;
}

inline bool descent_xml_validate_element(struct descent_xml_lex token)
{
	return descent_xml_validate_element_depth(token, 10000);
}

inline bool _descent_xml_non_space_text(struct descent_xml_lex token)
{
	return token.type == descent_xml_classifier_text
		|| token.type == descent_xml_classifier_text_entity_start
		|| token.type == descent_xml_classifier_text_entity;
}

inline struct descent_xml_lex _descent_xml_validate_xmldecl(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context_p
)
{
	struct {
		bool valid;
		int depth;
	} *context = context_p;

	const struct libadt_const_lptr xmldecl = libadt_str_literal("?xml");

	if (libadt_const_lptr_equal(element_name, xmldecl)) {
		if (!empty) {
			context->valid = false;
			return token;
		}

		while (token.type != descent_xml_classifier_element) {
			if (
				token.type == descent_xml_classifier_unexpected
				|| token.type == descent_xml_classifier_eof
				|| _descent_xml_non_space_text(token)
			) {
				context->valid = false;
				return token;
			}
			token = descent_xml_lex_next_raw(token);
		}

		struct descent_xml_lex next = descent_xml_lex_next_raw(token);

		if (next.type == descent_xml_lex_doctype) {
			while (token.type != descent_xml_classifier_element) {
				if (
					token.type == descent_xml_classifier_unexpected
					|| token.type == descent_xml_classifier_eof
					|| _descent_xml_non_space_text(token)
				) {
					context->valid = false;
					return token;
				}
				token = descent_xml_lex_next_raw(token);
			}
		}

		return descent_xml_parse(
			token,
			_descent_xml_validate_element_handler,
			NULL,
			context
		);
	}

	return _descent_xml_validate_element_handler(
		token,
		element_name,
		attributes,
		empty,
		context
	);
}

inline bool descent_xml_validate_document_depth(
	struct descent_xml_lex token,
	int depth
)
{
	struct {
		bool valid;
		int depth;
	} context = {true, depth};
	while (token.type != descent_xml_classifier_element) {
		if (
			token.type == descent_xml_classifier_unexpected
			|| token.type == descent_xml_classifier_eof
		)
			return false;
		token = descent_xml_lex_next_raw(token);
	}

	token = descent_xml_parse(
		token,
		_descent_xml_validate_xmldecl,
		NULL,
		&context
	);

	if (!context.valid)
		return false;

	// check that there's only one element node in the root
	while (token.type != descent_xml_classifier_element) {
		if (token.type == descent_xml_classifier_eof)
			return true;
		if (token.type == descent_xml_classifier_unexpected)
			return false;
		if (token.type == descent_xml_classifier_text)
			return false;
		token = descent_xml_lex_next_raw(token);
	}

	return false;
}

inline bool descent_xml_validate_document(
	struct descent_xml_lex token
)
{
	return descent_xml_validate_document_depth(token, 1000);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DESCENT_XML_VALIDATE
