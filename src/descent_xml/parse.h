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

#ifndef DESCENT_XML_PARSE
#define DESCENT_XML_PARSE

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


/**
 * \brief Type signature for a user-passed element parsing function.
 * 	Used by descent_xml_parse().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with descent_xml_lex_next_raw(),
 * 	or passed to a recursive call to descent_xml_parse().
 * \param element_name An libadt_const_lptr to the byte array of the
 * 	element name. element_name.length contains the number of
 * 	bytes for the name and the pointer can be retrieved with
 * 	libadt_const_lptr_raw(element_name).
 * \param attributes An libadt_const_lptr of libadt_const_lptrs to
 * 	attribute strings. attributes.length contains the number of
 * 	strings. The zeroth string will be the zeroth attribute name,
 * 	the first string will be its value; the second string will be
 * 	the first attribute name, the third its value, and so on.
 * \param empty True if the element is an empty element, of the format
 * 	`<element-name />`. False if the element is terminated with a
 * 	closing tag. Note that elements with no content between an opening
 * 	and closing tag are _not_ considered empty elements.
 * \param context The pointer provided to descent_xml_parse() by the user.
 *
 * \returns The last token processed. This can be the token passed
 * 	in as an argument, or a token generated as a result of further
 * 	processing inside the function.
 */
typedef struct descent_xml_lex descent_xml_parse_element_fn(
	struct descent_xml_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);

/**
 * \brief Type signature for a user-passed text node parsing function.
 * 	Used by descent_xml_parse().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with descent_xml_lex_next_raw(),
 * 	or passed to a recursive call to descent_xml_parse().
 * \param text A length-pointer containing the text.
 * \param context The pointer provided to descent_xml_parse() by the user.
 *
 * \returns The last token processed. This can be the token passed
 * 	in as an argument, or a token generated as a result of further
 * 	processing inside the function.
 */
typedef struct descent_xml_lex descent_xml_parse_text_fn(
	struct descent_xml_lex token,
	struct libadt_const_lptr text,
	void *context
);

/**
 * \brief Type signature for a user-passed element parsing function.
 * 	Used by descent_xml_parse_cstr().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with descent_xml_lex_next_raw(),
 * 	or passed to a recursive call to descent_xml_parse().
 * \param element_name A null-terminated string containing the element
 * 	name.
 * \param attributes A null-terminated array of null-terminated strings.
 * 	The zeroth string will be the zeroth attribute name, the first
 * 	string will be its value; the second will be the first attribute
 * 	name, the third its value, and so on.
 * \param empty True if the element is an empty element, of the format
 * 	`<element-name />`. False if the element is terminated with a
 * 	closing tag. Note that elements with no content between an opening
 * 	and closing tag are _not_ considered empty elements.
 * \param context The pointer provided to descent_xml_parse_cstr() by the user.
 */
typedef struct descent_xml_lex descent_xml_parse_element_cstr_fn(
	struct descent_xml_lex token,
	char *element_name,
	char **attributes,
	bool empty,
	void *context
);

/**
 * \brief Type signature for a user-passed text node parsing function.
 * 	Used by descent_xml_parse_cstr().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with descent_xml_lex_next_raw(),
 * 	or passed to a recursive call to descent_xml_parse_cstr().
 * \param text A pointer to a null-terminated string containing the text.
 * \param context The pointer provided to descent_xml_parse_cstr() by the user.
 */
typedef struct descent_xml_lex descent_xml_parse_text_cstr_fn(
	struct descent_xml_lex token,
	char *text,
	void *context
);

inline bool _descent_xml_end_token(struct descent_xml_lex token)
{
	return token.type == descent_xml_classifier_eof
		|| token.type == descent_xml_classifier_unexpected;
}

inline bool _descent_xml_is_attribute_value_type(struct descent_xml_lex token)
{
	return token.type == descent_xml_classifier_attribute_value_single_quote
		|| token.type == descent_xml_classifier_attribute_value_single_quote_entity_start
		|| token.type == descent_xml_classifier_attribute_value_single_quote_entity
		|| token.type == descent_xml_classifier_attribute_value_double_quote
		|| token.type == descent_xml_classifier_attribute_value_double_quote_entity_start
		|| token.type == descent_xml_classifier_attribute_value_double_quote_entity;

}

typedef struct {
	struct libadt_const_lptr value;
	struct descent_xml_lex token;
} _descent_xml_value_t;

inline _descent_xml_value_t _descent_xml_attribute_value(
	struct descent_xml_lex token
)
{
	if (!_descent_xml_is_attribute_value_type(token))
		return (_descent_xml_value_t) {
			libadt_const_lptr_truncate(token.value, 0),
			token
		};

	struct libadt_const_lptr result = token.value;
	struct descent_xml_lex next = descent_xml_lex_next_raw(token);
	while (_descent_xml_is_attribute_value_type(next)) {
		result.length += next.value.length;
		next = descent_xml_lex_next_raw(next);
	}
	return (_descent_xml_value_t) { result, next };
}

inline struct descent_xml_lex _descent_xml_handle_element(
	struct descent_xml_lex token,
	descent_xml_parse_element_fn *element_handler,
	void *context
)
{
	const struct libadt_const_lptr name = token.value;

	token = descent_xml_lex_next_raw(token);
	if (token.type == descent_xml_classifier_unexpected)
		return token;

	LIBADT_VECTOR_WITH(attributes, sizeof(struct libadt_const_lptr), 0) {
		while (token.type == descent_xml_classifier_element_space) {
			token = descent_xml_lex_next_raw(token);

			if (token.type == descent_xml_classifier_attribute_name) {
				attributes = libadt_vector_append(
					attributes,
					&token.value
				);
				token = descent_xml_lex_next_raw(token);
				if (token.type == descent_xml_classifier_attribute_expect_assign)
					token = descent_xml_lex_next_raw(token);
				if (token.type == descent_xml_classifier_attribute_assign)
					token = descent_xml_lex_next_raw(token);
				const bool quote =
					token.type == descent_xml_classifier_attribute_value_single_quote_start
					|| token.type == descent_xml_classifier_attribute_value_double_quote_start;
				if (quote)
					token = descent_xml_lex_next_raw(token);

				_descent_xml_value_t attr
					= _descent_xml_attribute_value(token);
				attributes = libadt_vector_append(
					attributes,
					&attr.value
				);
				token = attr.token;
				token = descent_xml_lex_next_raw(token);
			}
		}

		if (token.type == descent_xml_classifier_unexpected)
			continue;

		const bool is_empty
			= token.type == descent_xml_classifier_element_empty;

		if (is_empty || token.type == descent_xml_classifier_element_end) {
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

inline bool _descent_xml_is_text_type(struct descent_xml_lex token)
{
	return token.type == descent_xml_classifier_text
		|| token.type == descent_xml_classifier_text_entity_start
		|| token.type == descent_xml_classifier_text_entity;
}

inline _descent_xml_value_t _descent_xml_text_value(
	struct descent_xml_lex token
)
{
	struct libadt_const_lptr result = token.value;
	struct descent_xml_lex next = descent_xml_lex_next_raw(token);
	while (_descent_xml_is_text_type(next)) {
		result.length += next.value.length;
		token = next;
		next = descent_xml_lex_next_raw(next);
	}
	return (_descent_xml_value_t) { result, token };
}

inline struct descent_xml_lex _descent_xml_handle_text(
	struct descent_xml_lex token,
	descent_xml_parse_text_fn *text_handler,
	void *context
)
{
	_descent_xml_value_t result = _descent_xml_text_value(token);
	return text_handler(result.token, result.value, context);
}

/**
 * \brief Function for parsing an XML document.
 *
 * descent_xml_parse() is the version of the parser that does not allocate
 * new memory and does not copy strings. Instead, it uses the
 * length-pointer implementation from libadt to point into the original
 * XML file for the element names, attributes and text. This also means
 * that entities are not converted, and the text passed to the callbacks
 * is not null-terminated.
 *
 * This function will only parse a single entity. If the entity is an
 * opening XML element, it will be parsed and passed to the given
 * element_handler. If the entity is a text node, it will be parsed and
 * passed to the text_handler. The return value will be the token
 * returned by a handler if called, or the next token to process if
 * neither were called.
 *
 * \param xml A token into an XML document. Can be created on a
 * 	full XML document using descent_xml_lex_init().
 * \param element_handler A callback to call when encountering an
 * 	opening element tag. Pass a NULL pointer to disable.
 * \param text_handler A callback to call when encountering a
 * 	text node. Pass a NULL pointer to disable.
 * \param context A user-provided pointer that will be passed
 * 	to the callbacks.
 *
 * \returns The last token encountered while parsing. If the
 * 	return value's `type` property is `descent_xml_classifier_unexpected`,
 * 	an error was encountered. If the `type` property is
 * 	`descent_xml_classifier_eof`, then the end of the XML was encountered
 * 	in an expected way.
 *
 * \sa descent_xml_parse_cstr() An interface for C-style strings.
 */
DESCENT_XML_EXPORT inline struct descent_xml_lex descent_xml_parse(
	struct descent_xml_lex xml,
	descent_xml_parse_element_fn *element_handler,
	descent_xml_parse_text_fn *text_handler,
	void *context
)
{
	xml = descent_xml_lex_next_raw(xml);

	if (xml.type == descent_xml_classifier_element_name && element_handler) {
		xml = _descent_xml_handle_element(
			xml,
			element_handler,
			context
		);
	} else if (_descent_xml_is_text_type(xml) && text_handler) {
		xml = _descent_xml_handle_text(
			xml,
			text_handler,
			context
		);
	}

	return xml;
}

typedef struct {
	descent_xml_parse_element_cstr_fn *element_handler;
	descent_xml_parse_text_cstr_fn *text_handler;
	void *context;
} _descent_xml_parse_cstr_context;

DESCENT_XML_EXPORT extern descent_xml_classifier_void_fn *descent_xml_parse_error(wchar_t);

inline struct descent_xml_lex _cstr_element_handler(
	struct descent_xml_lex xml,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
)
{
	// this function is why I hate c-strings
	const _descent_xml_parse_cstr_context *const cstr_context = context;
	if (!cstr_context->element_handler)
		return xml;

	char *const cname = strndup(element_name.buffer, (size_t)element_name.length);
	if (!cname)
		goto error_return_xml;

	char * *const cattr = calloc((size_t)(attributes.length + 1), sizeof(char*));
	if (!cattr)
		goto error_free_cname;

	for (ssize_t i = 0; i < attributes.length; ++i) {
		const struct libadt_const_lptr *const attarr = attributes.buffer;
		const struct libadt_const_lptr *const attribute = &attarr[i];
		cattr[i] = strndup(attribute->buffer, (size_t)attribute->length);
		if (!cattr[i])
			goto error_free_cattr;
	}

	xml = cstr_context->element_handler(
		xml,
		cname,
		cattr,
		empty,
		cstr_context->context
	);

	for (char **attr = cattr; *attr; attr++) {
		free(*attr);
	}
	free(cattr);
	free(cname);

	return xml;

error_free_cattr:
	for (char **attr = cattr; *attr; attr++) {
		free(*attr);
	}
	free(cattr);
error_free_cname:
	free(cname);
error_return_xml:
	xml.type = descent_xml_parse_error;
	return xml;
}

inline struct descent_xml_lex _cstr_text_handler(
	struct descent_xml_lex xml,
	struct libadt_const_lptr text,
	void *context
)
{
	const _descent_xml_parse_cstr_context *const cstr_context = context;
	if (!cstr_context->text_handler)
		return xml;

	char *const ctext = strndup(text.buffer, (size_t)text.length);
	if (!ctext) {
		xml.type = descent_xml_parse_error;
		return xml;
	}

	xml = cstr_context->text_handler(
		xml,
		ctext,
		cstr_context->context
	);

	free(ctext);
	return xml;
}

/**
 * \brief Function for parsing an XML document.
 *
 * descent_xml_parse_cstr() is the version of the parser that allocates
 * memory to copy values into. The strings are null-terminated
 * char arrays, and are freed by the parser after the relevant callback
 * is finished running.
 *
 * Entities are not converted.
 *
 * This function will only parse a single entity. If the entity is an
 * opening XML element, it will be parsed and passed to the given
 * element_handler. If the entity is a text node, it will be parsed
 * and passed to text_handler. The return value will be the token returned
 * by a handler if called, or the next token to process if neither were
 * called.
 *
 * \param xml A token into an XML document. Can be created on a full
 * 	XML document using descent_xml_lex_init().
 * \param element_handler A callback to call when encountering an opening
 * 	element tag. Pass a NULL pointer to disable.
 * \param text_handler A callback to call when encountering a text node.
 * 	Pass a NULL pointer to disable.
 * \param context A user-provided void pointer that will be passed to
 * 	the callbacks.
 *
 * \returns The last token encountered while parsing. If the return value's
 * 	`type` property is `descent_xml_classifier_unexpected`, a lex error
 * 	occurred. If the `type` property is
 * 	`descent_xml_parse_error`, there was an error allocating memory for
 * 	a value. If the `type` property is `descent_xml_classifier_eof`, the
 * 	end of the XML was encountered in an expected way.
 *
 * \sa descent_xml_parse() An interface using pointer-length structs,
 * 	using no allocation or copying logic.
 */
DESCENT_XML_EXPORT inline struct descent_xml_lex descent_xml_parse_cstr(
	struct descent_xml_lex xml,
	descent_xml_parse_element_cstr_fn *element_handler,
	descent_xml_parse_text_cstr_fn *text_handler,
	void *context
)
{
	_descent_xml_parse_cstr_context cstr_context = {
		.element_handler = element_handler,
		.text_handler = text_handler,
		.context = context,
	};
	return descent_xml_parse(
		xml,
		_cstr_element_handler,
		_cstr_text_handler,
		&cstr_context
	);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DESCENT_XML_PARSE
