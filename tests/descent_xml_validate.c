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
#include "descent_xml/validate.h"

#include <libadt/str.h>

typedef struct descent_xml_lex lex_t;
typedef struct libadt_const_lptr lptr_t;

#define lex descent_xml_lex_init
#define lit libadt_str_literal

void test_valid(void)
{
	{
		lex_t valid = lex(lit("<foo><foo></foo><bar></bar></foo>"));
		assert(descent_xml_validate_element(valid));
		assert(descent_xml_validate_document(valid));
	}

	{
		lex_t valid = lex(lit("<foo />"));
		assert(descent_xml_validate_element(valid));
		assert(descent_xml_validate_document(valid));
	}

	{
		lex_t valid = lex(lit("<?xml version=\"1.0\"?>\n<foo></foo>"));
		assert(descent_xml_validate_document(valid));
	}

	{
		lex_t valid = lex(lit("<!DOCTYPE html=\"\">\n<html></html>"));
		assert(descent_xml_validate_document(valid));
	}

	{
		lex_t valid = lex(lit("<?xml version=\"1.0\"?>\n<!DOCTYPE html=\"\">\n<html></html>"));
		assert(descent_xml_validate_document(valid));
	}
}

void test_invalid(void)
{
	{
		lex_t invalid = lex(lit("<foo></bar>"));
		assert(!descent_xml_validate_element(invalid));
	}

	{
		lex_t invalid = lex(lit("<foo><bar></bar>"));
		assert(!descent_xml_validate_element(invalid));
	}

	{
		lex_t invalid = lex(lit("<foo><bar></bar></bar>"));
		assert(!descent_xml_validate_element(invalid));
	}

	{
		lex_t invalid = lex(lit("<?xml version=\"1.0\"?>"));
		assert(!descent_xml_validate_document(invalid));
	}

	{
		lex_t invalid = lex(lit("<!DOCTYPE html=\"\">"));
		assert(!descent_xml_validate_document(invalid));
	}

	{
		lex_t invalid = lex(lit(""));
		assert(!descent_xml_validate_document(invalid));
	}

	{
		lex_t invalid = lex(lit("<!DOCTYPE html=\"\"><?xml?>"));
		assert(!descent_xml_validate_document(invalid));
	}

	{
		lex_t invalid = lex(lit("<foo></foo><bar></bar>"));
		assert(!descent_xml_validate_document(invalid));
	}
}

int main()
{
	test_valid();
	test_invalid();
}
