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

#ifndef XMLTREE_INIT
#define XMLTREE_INIT

// Mark functions as external
// We don't have compiler-specific flags
// here: just assume the human compiling
// the project knows the compiler and has
// configured the environment.
//
// If the environment isn't set up, just use
// the compiler defaults.
//
// For example: for GCC, set
// CFLAGS+="-fvisibility=hidden -fvisibility-inline-hidden -DDESCENT_XML_EXPORT='"'__attribute__((visibility("default")))'"'"
#ifndef DESCENT_XML_EXPORT
#define DESCENT_XML_EXPORT
#endif

#endif // XMLTREE_INIT
