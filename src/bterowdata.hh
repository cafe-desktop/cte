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

#pragma once

#include <string.h>

#include "bteunistr.h"
#include "btemacros.h"
#include "btedefines.hh"

#include "attr.hh"
#include "cell.hh"

G_BEGIN_DECLS

/*
 * BteRowAttr: A single row's attributes
 */

typedef struct _BteRowAttr {
        guint8 soft_wrapped  : 1;
        guint8 bidi_flags    : 4;
} BteRowAttr;
static_assert(sizeof (BteRowAttr) == 1, "BteRowAttr has wrong size");

/*
 * BteRowData: A single row's data
 */

typedef struct _BteRowData {
	BteCell *cells;
	guint16 len;
	BteRowAttr attr;
} BteRowData;


#define _bte_row_data_length(__row)			((__row)->len + 0)

static inline const BteCell *
_bte_row_data_get (const BteRowData *row, gulong col)
{
	if (G_UNLIKELY (row->len <= col))
		return NULL;

	return &row->cells[col];
}

static inline BteCell *
_bte_row_data_get_writable (BteRowData *row, gulong col)
{
	if (G_UNLIKELY (row->len <= col))
		return NULL;

	return &row->cells[col];
}

void _bte_row_data_init (BteRowData *row);
void _bte_row_data_clear (BteRowData *row);
void _bte_row_data_fini (BteRowData *row);
void _bte_row_data_insert (BteRowData *row, gulong col, const BteCell *cell);
void _bte_row_data_append (BteRowData *row, const BteCell *cell);
void _bte_row_data_remove (BteRowData *row, gulong col);
void _bte_row_data_fill (BteRowData *row, const BteCell *cell, gulong len);
void _bte_row_data_shrink (BteRowData *row, gulong max_len);
void _bte_row_data_copy (const BteRowData *src, BteRowData *dst);
guint16 _bte_row_data_nonempty_length (const BteRowData *row);

G_END_DECLS
