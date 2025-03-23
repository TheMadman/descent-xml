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
#include <stddef.h>
#include <wchar.h>

#include "xmltree/classifier.h"

typedef xmltree_classifier_void_fn vfn;
typedef xmltree_classifier_fn cfn;

static int expect(cfn *expected, cfn *state, wint_t input)
{
	return (cfn*)state((wchar_t)input) == expected;
}

void test_xmltree_classifier_start(void)
{
	assert(expect(
		xmltree_classifier_element,
		xmltree_classifier_start,
		'<'
	));
	assert(expect(
		xmltree_classifier_start,
		xmltree_classifier_start,
		' '
	));
	assert(expect(
		// expect the unexpected!
		// OK, that's the only time I'm making that joke
		xmltree_classifier_unexpected,
		xmltree_classifier_start,
		'a'
	));
}

void test_xmltree_classifier_element(void)
{
	assert(expect(
		xmltree_classifier_element_name,
		xmltree_classifier_element,
		'a'
	));
	assert(expect(
		xmltree_classifier_element_close,
		xmltree_classifier_element,
		'/'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_element,
		' '
	));
}

void test_xmltree_classifier_element_close(void)
{
	assert(expect(
		xmltree_classifier_element_close_name,
		xmltree_classifier_element_close,
		'a'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_element_close,
		' '
	));
}

void test_xmltree_classifier_element_close_name(void)
{
	assert(expect(
		xmltree_classifier_element_close_name,
		xmltree_classifier_element_close_name,
		'b'
	));
	assert(expect(
		xmltree_classifier_element_close_space,
		xmltree_classifier_element_close_name,
		' '
	));
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_element_close_name,
		'>'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_element_close_name,
		WEOF
	));
}

void test_xmltree_classifier_element_close_space(void)
{
	assert(expect(
		xmltree_classifier_element_close_space,
		xmltree_classifier_element_close_space,
		' '
	));
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_element_close_space,
		'>'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_element_close_space,
		'a'
	));
}

void test_xmltree_classifier_element_name(void)
{
	assert(expect(
		xmltree_classifier_element_name,
		xmltree_classifier_element_name,
		'a'
	));
	assert(expect(
		xmltree_classifier_element_space,
		xmltree_classifier_element_name,
		' '
	));
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_element_name,
		'>'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_element_name,
		'"'
	));
}

void test_xmltree_classifier_element_space(void)
{
	assert(expect(
		xmltree_classifier_attribute_name,
		xmltree_classifier_element_space,
		'a'
	));
	assert(expect(
		xmltree_classifier_element_space,
		xmltree_classifier_element_space,
		' '
	));
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_element_space,
		'>'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_element_space,
		'"'
	));
}

void test_xmltree_classifier_attribute_expect_assign(void)
{
	assert(expect(
		xmltree_classifier_attribute_assign,
		xmltree_classifier_attribute_expect_assign,
		'='
	));
	assert(expect(
		xmltree_classifier_attribute_expect_assign,
		xmltree_classifier_attribute_expect_assign,
		' '
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_expect_assign,
		'a'
	));
}

void test_xmltree_classifier_attribute_assign(void)
{
	assert(expect(
		xmltree_classifier_attribute_assign,
		xmltree_classifier_attribute_assign,
		' '
	));
	assert(expect(
		xmltree_classifier_attribute_value_single_quote,
		xmltree_classifier_attribute_assign,
		'\''
	));
	assert(expect(
		xmltree_classifier_attribute_value_double_quote,
		xmltree_classifier_attribute_assign,
		'"'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_assign,
		'a'
	));
}

void test_xmltree_classifier_attribute_value_single_quote(void)
{
	assert(expect(
		xmltree_classifier_attribute_value_single_quote_end,
		xmltree_classifier_attribute_value_single_quote,
		'\''
	));
	assert(expect(
		xmltree_classifier_attribute_value_single_quote_entity_start,
		xmltree_classifier_attribute_value_single_quote,
		'%'
	));
	assert(expect(
		xmltree_classifier_attribute_value_single_quote_entity_start,
		xmltree_classifier_attribute_value_single_quote,
		'&'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_single_quote,
		'<'
	));
}

void test_xmltree_classifier_attribute_value_single_quote_entity_start(void)
{
	assert(expect(
		xmltree_classifier_attribute_value_single_quote_entity,
		xmltree_classifier_attribute_value_single_quote_entity_start,
		'#'
	));
	assert(expect(
		xmltree_classifier_attribute_value_single_quote_entity,
		xmltree_classifier_attribute_value_single_quote_entity_start,
		'a'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_single_quote_entity_start,
		'>'
	));
}

void test_xmltree_classifier_attribute_value_single_quote_entity(void)
{
	assert(expect(
		xmltree_classifier_attribute_value_single_quote_entity,
		xmltree_classifier_attribute_value_single_quote_entity,
		'a'
	));
	assert(expect(
		xmltree_classifier_attribute_value_single_quote,
		xmltree_classifier_attribute_value_single_quote_entity,
		';'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_single_quote_entity,
		' '
	));
}

void test_xmltree_classifier_attribute_value_single_quote_end(void)
{
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_attribute_value_single_quote_end,
		'>'
	));
	assert(expect(
		xmltree_classifier_element_space,
		xmltree_classifier_attribute_value_single_quote_end,
		' '
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_single_quote_end,
		'a'
	));
}

void test_xmltree_classifier_attribute_value_double_quote(void)
{
	assert(expect(
		xmltree_classifier_attribute_value_double_quote_end,
		xmltree_classifier_attribute_value_double_quote,
		'"'
	));
	assert(expect(
		xmltree_classifier_attribute_value_double_quote_entity_start,
		xmltree_classifier_attribute_value_double_quote,
		'%'
	));
	assert(expect(
		xmltree_classifier_attribute_value_double_quote_entity_start,
		xmltree_classifier_attribute_value_double_quote,
		'&'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_double_quote,
		'<'
	));
}

void test_xmltree_classifier_attribute_value_double_quote_entity_start(void)
{
	assert(expect(
		xmltree_classifier_attribute_value_double_quote_entity,
		xmltree_classifier_attribute_value_double_quote_entity_start,
		'#'
	));
	assert(expect(
		xmltree_classifier_attribute_value_double_quote_entity,
		xmltree_classifier_attribute_value_double_quote_entity_start,
		'a'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_double_quote_entity_start,
		'>'
	));
}

void test_xmltree_classifier_attribute_value_double_quote_entity(void)
{
	assert(expect(
		xmltree_classifier_attribute_value_double_quote_entity,
		xmltree_classifier_attribute_value_double_quote_entity,
		'a'
	));
	assert(expect(
		xmltree_classifier_attribute_value_double_quote,
		xmltree_classifier_attribute_value_double_quote_entity,
		';'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_double_quote_entity,
		' '
	));
}

void test_xmltree_classifier_attribute_value_double_quote_end(void)
{
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_attribute_value_double_quote_end,
		'>'
	));
	assert(expect(
		xmltree_classifier_element_space,
		xmltree_classifier_attribute_value_double_quote_end,
		' '
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_attribute_value_double_quote_end,
		'a'
	));
}

void test_xmltree_classifier_text(void)
{
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_text,
		'a'
	));
	assert(expect(
		xmltree_classifier_element,
		xmltree_classifier_text,
		'<'
	));
	assert(expect(
		xmltree_classifier_text_entity_start,
		xmltree_classifier_text,
		'&'
	));
	assert(expect(
		xmltree_classifier_text_entity_start,
		xmltree_classifier_text,
		'%'
	));
	assert(expect(
		xmltree_classifier_text_entity_start,
		xmltree_classifier_text,
		'&'
	));
	assert(expect(
		xmltree_classifier_eof,
		xmltree_classifier_text,
		WEOF
	));
	assert(expect(
		xmltree_classifier_eof,
		xmltree_classifier_text,
		WEOF
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_text,
		'>'
	));
}

void test_xmltree_classifier_text_entity_start(void)
{
	assert(expect(
		xmltree_classifier_text_entity,
		xmltree_classifier_text_entity_start,
		'#'
	));
	assert(expect(
		xmltree_classifier_text_entity,
		xmltree_classifier_text_entity_start,
		'a'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_text_entity_start,
		'>'
	));
}

void test_xmltree_classifier_text_entity(void)
{
	assert(expect(
		xmltree_classifier_text_entity,
		xmltree_classifier_text_entity,
		'a'
	));
	assert(expect(
		xmltree_classifier_text,
		xmltree_classifier_text_entity,
		';'
	));
	assert(expect(
		xmltree_classifier_unexpected,
		xmltree_classifier_text_entity,
		' '
	));
}

int main()
{
	test_xmltree_classifier_start();
	test_xmltree_classifier_element();
	test_xmltree_classifier_element_close();
	test_xmltree_classifier_element_close_name();
	test_xmltree_classifier_element_close_space();
	test_xmltree_classifier_element_name();
	test_xmltree_classifier_element_space();
	test_xmltree_classifier_attribute_expect_assign();
	test_xmltree_classifier_attribute_assign();
	test_xmltree_classifier_attribute_value_single_quote();
	test_xmltree_classifier_attribute_value_single_quote_entity_start();
	test_xmltree_classifier_attribute_value_single_quote_end();
	test_xmltree_classifier_attribute_value_single_quote_entity();
	test_xmltree_classifier_attribute_value_double_quote();
	test_xmltree_classifier_attribute_value_double_quote_entity_start();
	test_xmltree_classifier_attribute_value_double_quote_entity();
	test_xmltree_classifier_attribute_value_double_quote_end();
	test_xmltree_classifier_text();
	test_xmltree_classifier_text_entity_start();
	test_xmltree_classifier_text_entity();
}
