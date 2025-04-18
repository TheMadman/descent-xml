/*
 * Project Name - Project Description
 * Copyright (C) 2025
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
#include "xmltree/lex.h"

#include <libadt/str.h>

#define lit libadt_str_literal

#define SCRIPT "<my-root foo=\"bar\" bar='baz' />"

void test_xmltree_lex(void)
{
	struct xmltree_lex token = xmltree_lex_init(lit(SCRIPT));

	assert(token.script.length == sizeof(SCRIPT) - 1);
}

void test_xmltree_lex_next_raw(void)
{
	struct xmltree_lex token = xmltree_lex_init(lit(SCRIPT));

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element_name);
	assert(token.value.length == sizeof("my-root") - 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element_space);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_name);
	assert(token.value.length == sizeof("foo") - 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_assign);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_value_double_quote_start);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_value_double_quote);
	assert(token.value.length == sizeof("bar") - 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_value_double_quote_end);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element_space);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_name);
	assert(token.value.length == sizeof("bar") - 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_assign);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_value_single_quote_start);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_value_single_quote);
	assert(token.value.length == sizeof("baz") - 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_attribute_value_single_quote_end);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element_space);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element_empty);
	assert(token.value.length == 1);

	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_element_end);
	assert(token.value.length == 1);
	
	token = xmltree_lex_next_raw(token);
	assert(token.type == xmltree_classifier_eof);
	assert(token.value.length == 0);
}

int main()
{
	test_xmltree_lex();
	test_xmltree_lex_next_raw();
}
