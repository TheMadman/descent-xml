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
	void *context
)
{
	(void)attributes;
	if (empty)
		return token;

	bool *still_valid = context;
	while (token.type != descent_xml_classifier_element_close_name) {
		token = descent_xml_parse(
			token,
			_descent_xml_validate_element_handler,
			NULL,
			still_valid
		);
		if (
			token.type == descent_xml_classifier_unexpected
			|| token.type == descent_xml_classifier_eof
		) {
			*still_valid = false;
			return token;
		}
		if (!*still_valid)
			break;
	}

	*still_valid = *still_valid && libadt_const_lptr_equal(
		token.value,
		element_name
	);
	token = descent_xml_parse(token, NULL, NULL, NULL);
	return token;
}

DESCENT_XML_EXPORT inline bool descent_xml_validate_element(struct descent_xml_lex token)
{
	bool valid = true;
	token = descent_xml_parse(
		token,
		_descent_xml_validate_element_handler,
		NULL,
		&valid
	);

	// We have to check for unexpected/eof here in case the
	// element handler never runs
	return
		valid
		&& token.type != descent_xml_classifier_unexpected
		&& token.type != descent_xml_classifier_eof;
}

inline struct descent_xml_lex _descent_xml_validate_doctype(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
)
{
	bool *still_valid = context;
	const struct libadt_const_lptr
		doctypedecl = libadt_str_literal("!DOCTYPE");

	if (libadt_const_lptr_equal(element_name, doctypedecl)) {
		// the doctypedecl doesn't have an empty-element
		// end marker like /> or ?>, but it doesn't have a
		// close tag either because XML is stupid
		if (empty) {
			*still_valid = false;
			return token;
		}

		while (*still_valid && token.type != descent_xml_classifier_eof) {
			if (
				token.type == descent_xml_classifier_eof
				|| token.type == descent_xml_classifier_element_close
			) {
				*still_valid = false;
				return token;
			}

			token = descent_xml_parse(
				token,
				_descent_xml_validate_element_handler,
				NULL,
				context
			);
		}

		return token;
	}

	return _descent_xml_validate_element_handler(
		token,
		element_name,
		attributes,
		empty,
		context
	);
}

inline struct descent_xml_lex _descent_xml_validate_xmldecl(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
)
{
	bool *still_valid = context;
	const struct libadt_const_lptr xmldecl = libadt_str_literal("?xml");

	if (libadt_const_lptr_equal(element_name, xmldecl)) {
		if (!empty) {
			*still_valid = false;
			return token;
		}

		while (token.type != descent_xml_classifier_eof) {
			if (token.type == descent_xml_classifier_unexpected) {
				*still_valid = false;
				return token;
			}

			token = descent_xml_parse(
				token,
				_descent_xml_validate_doctype,
				NULL,
				context
			);
		}

		return token;
	}

	return _descent_xml_validate_doctype(
		token,
		element_name,
		attributes,
		empty,
		context
	);
}

DESCENT_XML_EXPORT inline bool descent_xml_validate_document(struct descent_xml_lex token)
{
	bool valid = true;
	while (token.type != descent_xml_classifier_eof) {
		if (token.type == descent_xml_classifier_unexpected)
			return false;

		token = descent_xml_parse(
			token,
			_descent_xml_validate_xmldecl,
			NULL,
			&valid
		);
	}

	// We have to check for unexpected/eof here in case the
	// element handler never runs
	return
		valid
		&& token.type != descent_xml_classifier_unexpected;

}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DESCENT_XML_VALIDATE
