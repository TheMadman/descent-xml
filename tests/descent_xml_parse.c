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

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "descent_xml/parse.h"

#include "descent_xml/lex.h"

#include <libadt/str.h>

typedef struct descent_xml_lex lex_t;
typedef struct libadt_const_lptr lptr_t;

#define lex descent_xml_lex_init
#define lit libadt_str_literal
#define raw libadt_const_lptr_raw
#define index libadt_const_lptr_index
#define eof descent_xml_classifier_eof
#define err descent_xml_classifier_unexpected

static bool stop_token(lex_t token)
{
	return token.type == eof
		|| token.type == err;
}

lex_t empty_callback(
	lex_t token,
	lptr_t name,
	lptr_t attributes,
	bool empty,
	void *context
)
{
	assert(strncmp(name.buffer, "empty", (size_t)name.length) == 0);
	assert(attributes.buffer == NULL);
	assert(empty);
	*(bool*)context = true;
	return token;
}

void test_empty_element_no_attributes(void)
{
	{
		lex_t xml = lex(lit("<empty/>"));
		bool ran = false;
		while (!stop_token(xml))
			xml = descent_xml_parse(xml, empty_callback, NULL, &ran);
		assert(ran);
		assert(xml.type != err);
	}

	{
		lex_t xml = lex(lit("<empty />"));
		bool ran = false;
		while (!stop_token(xml))
			xml = descent_xml_parse(xml, empty_callback, NULL, &ran);
		assert(ran);
		assert(xml.type != err);
	}
}

lex_t prologue_callback(
	lex_t token,
	lptr_t name,
	lptr_t attributes,
	bool empty,
	void *context
)
{
	(void)attributes;
	assert(empty);
	assert(strncmp(name.buffer, "?xml", (size_t)name.length) == 0);
	*(bool*)context = true;
	return token;
}

lex_t element_callback(
	lex_t token,
	lptr_t name,
	lptr_t attributes,
	bool empty,
	void *context
)
{
	assert(!empty);
	assert(strncmp(name.buffer, "element", (size_t)name.length) == 0);

	{
		const lptr_t *attribute_name = raw(index(attributes, 0));
		const lptr_t *attribute_value = raw(index(attributes, 1));

		assert(strncmp(attribute_name->buffer, "first", (size_t)attribute_name->length) == 0);
		assert(strncmp(attribute_value->buffer, "firstval", (size_t)attribute_value->length) == 0);
	}

	{
		const lptr_t *attribute_name = raw(index(attributes, 2));
		const lptr_t *attribute_value = raw(index(attributes, 3));

		assert(strncmp(attribute_name->buffer, "second", (size_t)attribute_name->length) == 0);
		assert(strncmp(attribute_value->buffer, "secondval", (size_t)attribute_value->length) == 0);
	}

	{
		const lptr_t *attribute_name = raw(index(attributes, 4));
		const lptr_t *attribute_value = raw(index(attributes, 5));

		assert(strncmp(attribute_name->buffer, "third", (size_t)attribute_name->length) == 0);
		assert(strncmp(attribute_value->buffer, "", (size_t)attribute_value->length) == 0);
	}

	(*(int*)context)++;
	return token;
}

void test_element_attributes(void)
{
	{
		lex_t xml = lex(lit("<?xml version=\"1.0\" ?>"));
		bool ran = false;
		while (!stop_token(xml))
			xml = descent_xml_parse(xml, prologue_callback, NULL, &ran);
		assert(ran);
		assert(xml.type != err);
	}

	{
		lex_t xml = lex(lit("<element first='firstval' second = \"secondval\" third=''></element>"));
		int run_times = 0;
		while (!stop_token(xml))
			xml = descent_xml_parse(xml, element_callback, NULL, &run_times);
		assert(run_times == 1);
		assert(xml.type != err);
	}
}

lex_t text_callback(
	lex_t token,
	lptr_t text,
	void *context
)
{
	*(bool*)context = true;
	assert(strncmp(text.buffer, "Hello, world!", (size_t)text.length) == 0);
	return token;
}

void test_text_node(void)
{
	{
		lex_t xml = lex(lit("<text>Hello, world!</text>"));
		bool ran = false;
		while (!stop_token(xml))
			xml = descent_xml_parse(xml, NULL, text_callback, &ran);
		assert(ran);
		assert(xml.type != err);
	}
}

lex_t element_attribute_entity_callback(
	lex_t token,
	lptr_t element,
	lptr_t attributes,
	bool empty,
	void *context
)
{
	(void)element;
	assert(empty);
	assert(strncmp(element.buffer, "element", (size_t)element.length) == 0);
	{
		const lptr_t *attr = raw(index(attributes, 0));
		const lptr_t *val = raw(index(attributes, 1));

		assert(strncmp(attr->buffer, "attr", (size_t)attr->length) == 0);
		assert(strncmp(val->buffer, "this &amp; that", (size_t)val->length) == 0);
	}

	{
		const lptr_t *attr = raw(index(attributes, 2));
		const lptr_t *val = raw(index(attributes, 3));

		assert(strncmp(attr->buffer, "attr2", (size_t)attr->length) == 0);
		assert(strncmp(val->buffer, "&amp; the other", (size_t)val->length) == 0);
	}

	*(bool*)context = true;
	return token;
}

void test_attribute_entities(void)
{
	lex_t xml = lex(lit("<element attr='this &amp; that' attr2='&amp; the other'/>"));
	bool ran = false;
	while (!stop_token(xml))
		xml = descent_xml_parse(xml, element_attribute_entity_callback, NULL, &ran);
	assert(ran);
	assert(xml.type != err);
}

lex_t text_entity_callback(
	lex_t token,
	lptr_t value,
	void *context
)
{
	assert(strncmp(value.buffer, "this &amp; that", (size_t)value.length) == 0);
	*(bool*)context = true;
	return token;
}

void test_text_entities(void)
{
	lex_t xml = lex(lit("<element>this &amp; that</element>"));
	bool ran = false;
	while (!stop_token(xml))
		xml = descent_xml_parse(xml, NULL, text_entity_callback, &ran);
	assert(ran);
	assert(xml.type != err);
}

lex_t cstr_empty_element_callback(
	lex_t token,
	char *name,
	char **attributes,
	bool empty,
	void *context
)
{
	*(bool*)context = true;
	assert(strcmp(name, "empty") == 0);
	assert(*attributes == NULL);
	assert(empty);
	return token;
}

void test_cstr_empty_element_no_attributes(void)
{
	lex_t xml = lex(lit("<empty/>"));
	bool ran = false;
	while (!stop_token(xml))
		xml = descent_xml_parse_cstr(xml, cstr_empty_element_callback, NULL, &ran);
	assert(ran);
	assert(xml.type != err);
}

lex_t cstr_element_attributes_callback(
	lex_t token,
	char *name,
	char **attributes,
	bool empty,
	void *context
)
{
	*(int*)context += 1;
	assert(strcmp(name, "element") == 0);
	assert(strcmp(attributes[0], "first") == 0);
	assert(strcmp(attributes[1], "firstval") == 0);
	assert(strcmp(attributes[2], "second") == 0);
	assert(strcmp(attributes[3], "secondval") == 0);
	assert(strcmp(attributes[4], "third") == 0);
	assert(strcmp(attributes[5], "") == 0);
	assert(!attributes[6]);
	assert(!empty);
	return token;
}

void test_cstr_element_attributes(void)
{
	{
		lex_t xml = lex(lit("<element first='firstval' second = \"secondval\" third=''></element>"));
		int run_times = 0;
		while (!stop_token(xml))
			xml = descent_xml_parse_cstr(xml, cstr_element_attributes_callback, NULL, &run_times);
		assert(run_times == 1);
		assert(xml.type != err);
	}
}

lex_t cstr_text_entity_callback(
	lex_t xml,
	char *text,
	void *context
)
{
	*(bool*)context = true;
	assert(strcmp(text, "this &amp; that") == 0);
	return xml;
}

void test_cstr_text_entities(void)
{
	lex_t xml = lex(lit("<element>this &amp; that</element>"));
	bool ran = false;
	while (!stop_token(xml))
		xml = descent_xml_parse_cstr(xml, NULL, cstr_text_entity_callback, &ran);
	assert(ran);
	assert(xml.type != err);
}

int main()
{
	test_empty_element_no_attributes();
	test_element_attributes();
	test_attribute_entities();
	test_text_node();
	test_text_entities();
	test_cstr_empty_element_no_attributes();
	test_cstr_element_attributes();
	test_cstr_text_entities();
}
