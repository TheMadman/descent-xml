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


/**
 * \brief Type signature for a user-passed element parsing function.
 * 	Used by xmltree_parse().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with xmltree_lex_next_raw(),
 * 	or passed to a recursive call to xmltree_parse().
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
 * \param context The pointer provided to xmltree_parse() by the user.
 *
 * \returns The last token processed. This can be the token passed
 * 	in as an argument, or a token generated as a result of further
 * 	processing inside the function.
 */
typedef struct xmltree_lex xmltree_parse_element_fn(
	struct xmltree_lex token,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
);

/**
 * \brief Type signature for a user-passed text node parsing function.
 * 	Used by xmltree_parse().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with xmltree_lex_next_raw(),
 * 	or passed to a recursive call to xmltree_parse().
 * \param text A length-pointer containing the text.
 * \param context The pointer provided to xmltree_parse() by the user.
 *
 * \returns The last token processed. This can be the token passed
 * 	in as an argument, or a token generated as a result of further
 * 	processing inside the function.
 */
typedef struct xmltree_lex xmltree_parse_text_fn(
	struct xmltree_lex token,
	struct libadt_const_lptr text,
	void *context
);

/**
 * \brief Type signature for a user-passed element parsing function.
 * 	Used by xmltree_parse_cstr().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with xmltree_lex_next_raw(),
 * 	or passed to a recursive call to xmltree_parse().
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
 * \param context The pointer provided to xmltree_parse_cstr() by the user.
 */
typedef struct xmltree_lex xmltree_parse_element_cstr_fn(
	struct xmltree_lex token,
	char *element_name,
	char **attributes,
	bool empty,
	void *context
);

/**
 * \brief Type signature for a user-passed text node parsing function.
 * 	Used by xmltree_parse().
 *
 * \param token The last token encountered by the parser. This can
 * 	be returned directly, iterated over with xmltree_lex_next_raw(),
 * 	or passed to a recursive call to xmltree_parse().
 * \param text A pointer to a null-terminated string containing the text.
 * \param context The pointer provided to xmltree_parse() by the user.
 */
typedef struct xmltree_lex xmltree_parse_text_cstr_fn(
	struct xmltree_lex token,
	char *text,
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

/**
 * \brief Function for parsing an XML document.
 *
 * xmltree_parse() is the version of the parser that does not allocate
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
 * 	full XML document using xmltree_lex_init().
 * \param element_handler A callback to call when encountering an
 * 	opening element tag. Pass a NULL pointer to disable.
 * \param text_handler A callback to call when encountering a
 * 	text node. Pass a NULL pointer to disable.
 * \param context A user-provided pointer that will be passed
 * 	to the callbacks.
 *
 * \returns The last token encountered while parsing. If the
 * 	return value's `type` property is `xmltree_classifier_unexpected`,
 * 	an error was encountered. If the `type` property is
 * 	`xmltree_classifier_eof`, then the end of the XML was encountered
 * 	in an expected way.
 *
 * \sa xmltree_parse_cstr() An interface for C-style strings.
 */
XMLTREE_EXPORT inline struct xmltree_lex xmltree_parse(
	struct xmltree_lex xml,
	xmltree_parse_element_fn *element_handler,
	xmltree_parse_text_fn *text_handler,
	void *context
)
{
	xml = xmltree_lex_next_raw(xml);

	if (xml.type == xmltree_classifier_element && element_handler) {
		xml = _xmltree_handle_element(
			xmltree_lex_next_raw(xml),
			element_handler,
			context
		);
	} else if (_xmltree_is_text_type(xml) && text_handler) {
		xml = _xmltree_handle_text(
			xml,
			text_handler,
			context
		);
	}

	return xml;
}

typedef struct {
	xmltree_parse_element_cstr_fn *element_handler;
	xmltree_parse_text_cstr_fn *text_handler;
	void *context;
} _xmltree_parse_cstr_context;

XMLTREE_EXPORT extern xmltree_classifier_void_fn *xmltree_parse_error(wchar_t);

inline struct xmltree_lex _cstr_element_handler(
	struct xmltree_lex xml,
	struct libadt_const_lptr element_name,
	struct libadt_const_lptr attributes,
	bool empty,
	void *context
)
{
	// this function is why I hate c-strings
	const _xmltree_parse_cstr_context *const cstr_context = context;
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
	xml.type = xmltree_parse_error;
	return xml;
}

inline struct xmltree_lex _cstr_text_handler(
	struct xmltree_lex xml,
	struct libadt_const_lptr text,
	void *context
)
{
	const _xmltree_parse_cstr_context *const cstr_context = context;
	if (!cstr_context->text_handler)
		return xml;

	char *const ctext = strndup(text.buffer, (size_t)text.length);
	if (!ctext) {
		xml.type = xmltree_parse_error;
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
 * xmltree_parse_cstr() is the version of the parser that allocates
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
 * 	XML document using xmltree_lex_init().
 * \param element_handler A callback to call when encountering an opening
 * 	element tag. Pass a NULL pointer to disable.
 * \param text_handler A callback to call when encountering a text node.
 * 	Pass a NULL pointer to disable.
 * \param context A user-provided void pointer that will be passed to
 * 	the callbacks.
 *
 * \returns The last token encountered while parsing. If the return value's
 * 	`type` property is `xmltree_classifier_unexpected`, a lex error
 * 	occurred. If the `type` property is
 * 	`xmltree_parse_error`, there was an error allocating memory for
 * 	a value. If the `type` property is `xmltree_classifier_eof`, the
 * 	end of the XML was encountered in an expected way.
 *
 * \sa xmltree_parse() An interface using pointer-length structs,
 * 	using no allocation or copying logic.
 */
XMLTREE_EXPORT inline struct xmltree_lex xmltree_parse_cstr(
	struct xmltree_lex xml,
	xmltree_parse_element_cstr_fn *element_handler,
	xmltree_parse_text_cstr_fn *text_handler,
	void *context
)
{
	_xmltree_parse_cstr_context cstr_context = {
		.element_handler = element_handler,
		.text_handler = text_handler,
		.context = context,
	};
	return xmltree_parse(
		xml,
		_cstr_element_handler,
		_cstr_text_handler,
		&cstr_context
	);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // XMLTREE_PARSE
