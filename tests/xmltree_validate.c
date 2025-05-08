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
#include "xmltree/validate.h"

#include <libadt/str.h>

typedef struct xmltree_lex lex_t;
typedef struct libadt_const_lptr lptr_t;

#define lex xmltree_lex_init
#define lit libadt_str_literal

void test_valid(void)
{
	{
		lex_t valid = lex(lit("<foo><foo></foo><bar></bar></foo>"));
		assert(xmltree_validate_element(valid));
	}

	{
		lex_t valid = lex(lit("<foo />"));
		assert(xmltree_validate_element(valid));
	}
}

void test_invalid(void)
{
	{
		lex_t invalid = lex(lit("<foo></bar>"));
		assert(!xmltree_validate_element(invalid));
	}

	{
		lex_t invalid = lex(lit("<foo><bar></bar>"));
		assert(!xmltree_validate_element(invalid));
	}

	{
		lex_t invalid = lex(lit("<foo><bar></bar></bar>"));
		assert(!xmltree_validate_element(invalid));
	}
}

int main()
{
	test_valid();
	test_invalid();
}
