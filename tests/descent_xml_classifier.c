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

#include "descent_xml/classifier.h"

typedef descent_xml_classifier_void_fn vfn;
typedef descent_xml_classifier_fn cfn;

static int expect(cfn *expected, cfn *state, wint_t input)
{
	return (cfn*)state((wchar_t)input) == expected;
}

void test_descent_xml_classifier_start(void)
{
	assert(expect(
		descent_xml_classifier_element,
		descent_xml_classifier_start,
		'<'
	));
	assert(expect(
		descent_xml_classifier_start,
		descent_xml_classifier_start,
		' '
	));
	assert(expect(
		// expect the unexpected!
		// OK, that's the only time I'm making that joke
		descent_xml_classifier_unexpected,
		descent_xml_classifier_start,
		'a'
	));
}

void test_descent_xml_classifier_element(void)
{
	assert(expect(
		descent_xml_classifier_element_name,
		descent_xml_classifier_element,
		'a'
	));
	assert(expect(
		descent_xml_classifier_element_close,
		descent_xml_classifier_element,
		'/'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element,
		' '
	));
}

void test_descent_xml_classifier_element_close(void)
{
	assert(expect(
		descent_xml_classifier_element_close_name,
		descent_xml_classifier_element_close,
		'a'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element_close,
		' '
	));
}

void test_descent_xml_classifier_element_close_name(void)
{
	assert(expect(
		descent_xml_classifier_element_close_name,
		descent_xml_classifier_element_close_name,
		'b'
	));
	assert(expect(
		descent_xml_classifier_element_close_space,
		descent_xml_classifier_element_close_name,
		' '
	));
	assert(expect(
		descent_xml_classifier_element_end,
		descent_xml_classifier_element_close_name,
		'>'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element_close_name,
		WEOF
	));
}

void test_descent_xml_classifier_element_close_space(void)
{
	assert(expect(
		descent_xml_classifier_element_close_space,
		descent_xml_classifier_element_close_space,
		' '
	));
	assert(expect(
		descent_xml_classifier_element_end,
		descent_xml_classifier_element_close_space,
		'>'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element_close_space,
		'a'
	));
}

void test_descent_xml_classifier_element_name(void)
{
	assert(expect(
		descent_xml_classifier_element_name,
		descent_xml_classifier_element_name,
		'a'
	));
	assert(expect(
		descent_xml_classifier_element_space,
		descent_xml_classifier_element_name,
		' '
	));
	assert(expect(
		descent_xml_classifier_element_end,
		descent_xml_classifier_element_name,
		'>'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element_name,
		'"'
	));
}

void test_descent_xml_classifier_element_space(void)
{
	assert(expect(
		descent_xml_classifier_attribute_name,
		descent_xml_classifier_element_space,
		'a'
	));
	assert(expect(
		descent_xml_classifier_element_space,
		descent_xml_classifier_element_space,
		' '
	));
	assert(expect(
		descent_xml_classifier_element_end,
		descent_xml_classifier_element_space,
		'>'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element_space,
		'"'
	));
}

void test_descent_xml_classifier_attribute_expect_assign(void)
{
	assert(expect(
		descent_xml_classifier_attribute_assign,
		descent_xml_classifier_attribute_expect_assign,
		'='
	));
	assert(expect(
		descent_xml_classifier_attribute_expect_assign,
		descent_xml_classifier_attribute_expect_assign,
		' '
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_expect_assign,
		'a'
	));
}

void test_descent_xml_classifier_attribute_assign(void)
{
	assert(expect(
		descent_xml_classifier_attribute_assign,
		descent_xml_classifier_attribute_assign,
		' '
	));
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_start,
		descent_xml_classifier_attribute_assign,
		'\''
	));
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_start,
		descent_xml_classifier_attribute_assign,
		'"'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_assign,
		'a'
	));
}

void test_descent_xml_classifier_attribute_value_single_quote(void)
{
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_end,
		descent_xml_classifier_attribute_value_single_quote,
		'\''
	));
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_entity_start,
		descent_xml_classifier_attribute_value_single_quote,
		'%'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_entity_start,
		descent_xml_classifier_attribute_value_single_quote,
		'&'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_single_quote,
		'<'
	));
}

void test_descent_xml_classifier_attribute_value_single_quote_entity_start(void)
{
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_entity,
		descent_xml_classifier_attribute_value_single_quote_entity_start,
		'#'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_entity,
		descent_xml_classifier_attribute_value_single_quote_entity_start,
		'a'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_single_quote_entity_start,
		'>'
	));
}

void test_descent_xml_classifier_attribute_value_single_quote_entity(void)
{
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote_entity,
		descent_xml_classifier_attribute_value_single_quote_entity,
		'a'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_single_quote,
		descent_xml_classifier_attribute_value_single_quote_entity,
		';'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_single_quote_entity,
		' '
	));
}

void test_descent_xml_classifier_attribute_value_single_quote_end(void)
{
	assert(expect(
		descent_xml_classifier_element_end,
		descent_xml_classifier_attribute_value_single_quote_end,
		'>'
	));
	assert(expect(
		descent_xml_classifier_element_space,
		descent_xml_classifier_attribute_value_single_quote_end,
		' '
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_single_quote_end,
		'a'
	));
}

void test_descent_xml_classifier_attribute_value_double_quote(void)
{
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_end,
		descent_xml_classifier_attribute_value_double_quote,
		'"'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_entity_start,
		descent_xml_classifier_attribute_value_double_quote,
		'%'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_entity_start,
		descent_xml_classifier_attribute_value_double_quote,
		'&'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_double_quote,
		'<'
	));
}

void test_descent_xml_classifier_attribute_value_double_quote_entity_start(void)
{
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_entity,
		descent_xml_classifier_attribute_value_double_quote_entity_start,
		'#'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_entity,
		descent_xml_classifier_attribute_value_double_quote_entity_start,
		'a'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_double_quote_entity_start,
		'>'
	));
}

void test_descent_xml_classifier_attribute_value_double_quote_entity(void)
{
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote_entity,
		descent_xml_classifier_attribute_value_double_quote_entity,
		'a'
	));
	assert(expect(
		descent_xml_classifier_attribute_value_double_quote,
		descent_xml_classifier_attribute_value_double_quote_entity,
		';'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_double_quote_entity,
		' '
	));
}

void test_descent_xml_classifier_attribute_value_double_quote_end(void)
{
	assert(expect(
		descent_xml_classifier_element_end,
		descent_xml_classifier_attribute_value_double_quote_end,
		'>'
	));
	assert(expect(
		descent_xml_classifier_element_space,
		descent_xml_classifier_attribute_value_double_quote_end,
		' '
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_attribute_value_double_quote_end,
		'a'
	));
}

void test_descent_xml_classifier_text(void)
{
	assert(expect(
		descent_xml_classifier_text,
		descent_xml_classifier_text,
		'a'
	));
	assert(expect(
		descent_xml_classifier_element,
		descent_xml_classifier_text,
		'<'
	));
	assert(expect(
		descent_xml_classifier_text_entity_start,
		descent_xml_classifier_text,
		'&'
	));
	assert(expect(
		descent_xml_classifier_text_entity_start,
		descent_xml_classifier_text,
		'%'
	));
	assert(expect(
		descent_xml_classifier_text_entity_start,
		descent_xml_classifier_text,
		'&'
	));
	assert(expect(
		descent_xml_classifier_eof,
		descent_xml_classifier_text,
		WEOF
	));
	assert(expect(
		descent_xml_classifier_eof,
		descent_xml_classifier_text,
		WEOF
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_text,
		'>'
	));
}

void test_descent_xml_classifier_text_entity_start(void)
{
	assert(expect(
		descent_xml_classifier_text_entity,
		descent_xml_classifier_text_entity_start,
		'#'
	));
	assert(expect(
		descent_xml_classifier_text_entity,
		descent_xml_classifier_text_entity_start,
		'a'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_text_entity_start,
		'>'
	));
}

void test_descent_xml_classifier_text_entity(void)
{
	assert(expect(
		descent_xml_classifier_text_entity,
		descent_xml_classifier_text_entity,
		'a'
	));
	assert(expect(
		descent_xml_classifier_text,
		descent_xml_classifier_text_entity,
		';'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_text_entity,
		' '
	));
}

void test_descent_xml_classifier_element_end(void)
{
	assert(expect(
		descent_xml_classifier_text,
		descent_xml_classifier_element_end,
		'a'
	));
	assert(expect(
		descent_xml_classifier_unexpected,
		descent_xml_classifier_element_end,
		'>'
	));
	assert(expect(
		descent_xml_classifier_element,
		descent_xml_classifier_element_end,
		'<'
	));
	assert(expect(
		descent_xml_classifier_eof,
		descent_xml_classifier_element_end,
		WEOF
	));
	assert(expect(
		descent_xml_classifier_text_entity_start,
		descent_xml_classifier_element_end,
		'%'
	));
	assert(expect(
		descent_xml_classifier_text_entity_start,
		descent_xml_classifier_element_end,
		'&'
	));
}

void test_descent_xml_classifier_text_space(void)
{
	assert(expect(
		descent_xml_classifier_text_space,
		descent_xml_classifier_text_space,
		' '
	));
	assert(expect(
		descent_xml_classifier_text_space,
		descent_xml_classifier_text_space,
		'\n'
	));
	assert(expect(
		descent_xml_classifier_text_space,
		descent_xml_classifier_text_space,
		'\t'
	));
}

int main()
{
	test_descent_xml_classifier_start();
	test_descent_xml_classifier_element();
	test_descent_xml_classifier_element_close();
	test_descent_xml_classifier_element_close_name();
	test_descent_xml_classifier_element_close_space();
	test_descent_xml_classifier_element_name();
	test_descent_xml_classifier_element_space();
	test_descent_xml_classifier_element_end();
	test_descent_xml_classifier_attribute_expect_assign();
	test_descent_xml_classifier_attribute_assign();
	test_descent_xml_classifier_attribute_value_single_quote();
	test_descent_xml_classifier_attribute_value_single_quote_entity_start();
	test_descent_xml_classifier_attribute_value_single_quote_end();
	test_descent_xml_classifier_attribute_value_single_quote_entity();
	test_descent_xml_classifier_attribute_value_double_quote();
	test_descent_xml_classifier_attribute_value_double_quote_entity_start();
	test_descent_xml_classifier_attribute_value_double_quote_entity();
	test_descent_xml_classifier_attribute_value_double_quote_end();
	test_descent_xml_classifier_text();
	test_descent_xml_classifier_text_entity_start();
	test_descent_xml_classifier_text_entity();
	test_descent_xml_classifier_text_space();
}
