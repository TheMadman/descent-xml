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

#ifndef XMLTREE_CLASSIFIER
#define XMLTREE_CLASSIFIER

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXPORT
#define EXPORT
#endif

/**
 * \file
 *
 * This file defines a finite state machine, where each
 * state is a function. The state takes an input character and
 * returns a pointer to the next function in the state
 * machine.
 *
 * If a state receives an input it's not expecting,
 * the xmltree_classifier_unexpected state is returned.
 * Check for this as an error value. Calling it is an
 * error and will call abort().
 *
 * If a state correctly reaches the end-of-file state,
 * the xmltree_classifier_eof state is returned. This means
 * the state machine ran to completion correctly. Check
 * for this to terminate your loop. Calling it is an error
 * and will call abort().
 */

#include <stddef.h>

/**
 * Type definition for a "void function".
 *
 * A pointer-to-void-function should be treated
 * the same as a pointer-to-void: the only meaningful
 * way to use it is to pass it as a parameter
 * to something else, or to cast it to a more
 * meaningful type.
 */
typedef void xmltree_classifier_void_fn(void);

/**
 * Type definition for a classifier function.
 *
 * A classifier function is a function receiving an input
 * character and returning a pointer to the next classifier function.
 */
typedef xmltree_classifier_void_fn *xmltree_classifier_fn(wchar_t input);

/**
 * Error value, indicating the classifier function
 * encountered an unexpected input.
 */
EXPORT extern xmltree_classifier_fn *const xmltree_classifier_unexpected;

/**
 * Return value indicating the classifier function
 * received an end-of-file (legitimately).
 */
EXPORT extern xmltree_classifier_fn *const xmltree_classifier_eof;

/**
 * Entry point for a script.
 *
 * Expected to be fed the beginning of the file.
 * This state will stay in this state for all whitespace
 * encountered, until an element beginning '<' is
 * encountered. If any other character is encountered,
 * it returns xmltree_classifier_unexpected.
 */
EXPORT xmltree_classifier_void_fn *xmltree_classifier_start(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_text(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element_name(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element_space(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element_empty_end(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_name(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_assign(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_single_quote(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_single_quote_end(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_single_quote_entity_start(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_single_quote_entity(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_double_quote(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_double_quote_end(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_double_quote_entity_start(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_value_double_quote_entity(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element_close(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element_close_name(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_element_close_space(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_text_entity_start(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_text_entity(wchar_t input);

EXPORT xmltree_classifier_void_fn *xmltree_classifier_attribute_expect_assign(wchar_t input);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // XMLTREE_LEX
