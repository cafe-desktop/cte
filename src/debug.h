/*
 * Copyright (C) 2002 Red Hat, Inc.
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
 */

/* The interfaces in this file are subject to change at any time. */

#ifndef bte_debug_h_included
#define bte_debug_h_included

#include <config.h>
#include <stdint.h>
#include <glib.h>

#ifndef BTE_COMPILATION
#define _bte_debug_flags _bte_external_debug_flags
#define _bte_debug_init  _bte_external_debug_init
#define _bte_debug_on    _bte_external_debug_on
#if !defined(__GNUC__) || !G_HAVE_GNUC_VARARGS
#define _bte_debug_print _bte_external_debug_print
#endif
#endif

G_BEGIN_DECLS

typedef enum {
	BTE_DEBUG_MISC		= 1 << 0,
	BTE_DEBUG_PARSER	= 1 << 1,
	BTE_DEBUG_IO		= 1 << 2,
	BTE_DEBUG_UPDATES	= 1 << 3,
	BTE_DEBUG_EVENTS	= 1 << 4,
	BTE_DEBUG_SIGNALS	= 1 << 5,
	BTE_DEBUG_SELECTION	= 1 << 6,
	BTE_DEBUG_SUBSTITUTION	= 1 << 7,
	BTE_DEBUG_RING		= 1 << 8,
	BTE_DEBUG_PTY		= 1 << 9,
	BTE_DEBUG_CURSOR	= 1 << 10,
	BTE_DEBUG_KEYBOARD	= 1 << 11,
	BTE_DEBUG_LIFECYCLE	= 1 << 12,
	BTE_DEBUG_WORK		= 1 << 13,
	BTE_DEBUG_CELLS		= 1 << 14,
	BTE_DEBUG_TIMEOUT	= 1 << 15,
	BTE_DEBUG_DRAW		= 1 << 16,
	BTE_DEBUG_ALLY		= 1 << 17,
	BTE_DEBUG_ADJ		= 1 << 18,
	BTE_DEBUG_PANGOCAIRO    = 1 << 19,
	BTE_DEBUG_WIDGET_SIZE   = 1 << 20,
        BTE_DEBUG_STYLE         = 1 << 21,
	BTE_DEBUG_RESIZE        = 1 << 22,
        BTE_DEBUG_REGEX         = 1 << 23,
        BTE_DEBUG_HYPERLINK     = 1 << 24,
        BTE_DEBUG_MODES         = 1 << 25,
        BTE_DEBUG_EMULATION     = 1 << 26,
        BTE_DEBUG_RINGVIEW      = 1 << 27,
        BTE_DEBUG_BIDI          = 1 << 28,
        BTE_DEBUG_CONVERSION    = 1 << 29,
        BTE_DEBUG_EXCEPTIONS    = 1 << 30,
} BteDebugFlags;

void _bte_debug_init(void);
const char *_bte_debug_sequence_to_string(const char *str,
                                          gssize length);

void _bte_debug_hexdump(char const* str,
                        uint8_t const* buf,
                        size_t len);

extern guint _bte_debug_flags;
static inline gboolean _bte_debug_on(guint flags) G_GNUC_CONST G_GNUC_UNUSED;

static inline gboolean
_bte_debug_on(guint flags)
{
	return (_bte_debug_flags & flags) != 0;
}

#ifdef BTE_DEBUG
#define _BTE_DEBUG_IF(flags) if (G_UNLIKELY (_bte_debug_on (flags)))
#else
#define _BTE_DEBUG_IF(flags) if (0)
#endif

#ifdef BTE_DEBUG
#if defined(__GNUC__) && G_HAVE_GNUC_VARARGS
#define _bte_debug_print(flags, fmt, ...) \
	G_STMT_START { _BTE_DEBUG_IF(flags) g_printerr(fmt, ##__VA_ARGS__); } G_STMT_END
#else
#include <stdarg.h>
static void _bte_debug_print(guint flags, const char *fmt, ...)
{
	_BTE_DEBUG_IF(flags) {
		va_list  ap;
		va_start (ap, fmt);
		g_vfprintf (stderr, fmt, ap);
		va_end (ap);
	}
}
#endif
#else
#define _bte_debug_print(args...) do { } while(0)
#endif /* BTE_DEBUG */

G_END_DECLS

#endif
