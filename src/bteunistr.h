/*
 * Copyright (C) 2008 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author(s):
 * 	Behdad Esfahbod
 */

#ifndef bte_bteunistr_h_included
#define bte_bteunistr_h_included

#include <glib.h>

G_BEGIN_DECLS

/**
 * bteunistr:
 *
 * bteunistr is a gunichar-compatible way to store strings.  A string
 * consisting of a single unichar c is represented as the same value
 * as c itself.  In that sense, gunichars can be readily used as
 * bteunistrs.  Longer strings can be built by appending a unichar
 * to an already existing string.
 *
 * bteunistr is essentially just a gunicode-compatible quark value.
 * It can be used to store strings (of a base followed by combining
 * characters) where the code was designed to only allow one character.
 *
 * Strings are internalized efficiently and never freed.  No memory
 * management of bteunistr values is needed.
 **/
typedef guint32 bteunistr;

/**
 * _bte_unistr_append_unichar:
 * @s: a #bteunistr
 * @c: Unicode character to append to @s
 *
 * Creates a bteunistr value for the string @s followed by the
 * character @c.
 *
 * Returns: the new #bteunistr value
 **/
bteunistr
_bte_unistr_append_unichar (bteunistr s, gunichar c);

/**
 * _bte_unistr_append_unistr:
 * @s: a #bteunistr
 * @t: another #bteunistr to append to @s
 *
 * Creates a bteunistr value for the string @s followed by the
 * string @t.
 *
 * Returns: the new #bteunistr value
 **/
bteunistr
_bte_unistr_append_unistr (bteunistr s, bteunistr t);

gunichar
_bte_unistr_get_base (bteunistr s);

/**
 * _bte_unistr_append_to_string:
 * @s: a #bteunistr
 * @c: Unicode character to replace the base character of @s.
 *
 * Creates a bteunistr value where the base character from @s is
 * replaced by @c, while the combining characters from @s are carried over.
 *
 * Returns: the new #bteunistr value
 */
bteunistr
_bte_unistr_replace_base (bteunistr s, gunichar c);

/**
 * _bte_unistr_append_to_string:
 * @s: a #bteunistr
 * @gs: a #GString to append @s to
 *
 * Appends @s to @gs.  This is how one converts a #bteunistr to a
 * traditional string.
 **/
void
_bte_unistr_append_to_string (bteunistr s, GString *gs);

/**
 * _bte_unistr_append_to_gunichars:
 * @s: a #bteunistr
 * @a: a #GArray of #gunichar items to append @s to
 *
 * Appends @s to @a.
 **/
void
_bte_unistr_append_to_gunichars (bteunistr s, GArray *a);

/**
 * _bte_unistr_strlen:
 * @s: a #bteunistr
 *
 * Counts the number of character in @s.
 *
 * Returns: length of @s in characters.
 **/
int
_bte_unistr_strlen (bteunistr s);

G_END_DECLS

#endif
