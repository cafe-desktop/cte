/*
 * Copyright (C) 2002,2009 Red Hat, Inc.
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
 * Red Hat Author(s): Nalin Dahyabhai, Behdad Esfahbod
 */

#include <config.h>

#include "debug.h"
#include "bterowdata.hh"

#include <string.h>

#include <type_traits>

/* This will be true now that BteCell is POD, but make sure it'll be true
 * once that changes.
 */
static_assert(std::is_trivially_copy_constructible<BteCell>::value, "BteCell is not copy constructible");
static_assert(std::is_trivially_move_constructible<BteCell>::value, "BteCell is not move constructible");
static_assert(std::is_trivially_copyable<BteCell>::value, "BteCell is not trivially copyable");
static_assert(std::is_trivially_copy_assignable<BteCell>::value, "BteCell is not copy assignable");
static_assert(std::is_trivially_move_assignable<BteCell>::value, "BteCell is not move assignable");

/*
 * BteCells: A row's cell array
 */

typedef struct _BteCells BteCells;
struct _BteCells {
	guint32 alloc_len;
	BteCell cells[1];
};

static inline BteCells *
_bte_cells_for_cell_array (BteCell *cells)
{
	if (G_UNLIKELY (!cells))
		return NULL;

	return (BteCells *) (((guchar *) cells) - G_STRUCT_OFFSET (BteCells, cells));
}

static BteCells *
_bte_cells_realloc (BteCells *cells, guint32 len)
{
	guint32 alloc_len = (1 << g_bit_storage (MAX (len, 80))) - 1;

	_bte_debug_print(BTE_DEBUG_RING, "Enlarging cell array of %d cells to %d cells\n", cells ? cells->alloc_len : 0, alloc_len);
	cells = (BteCells *)g_realloc (cells, G_STRUCT_OFFSET (BteCells, cells) + alloc_len * sizeof (cells->cells[0]));
	cells->alloc_len = alloc_len;

	return cells;
}

static void
_bte_cells_free (BteCells *cells)
{
	_bte_debug_print(BTE_DEBUG_RING, "Freeing cell array of %d cells\n", cells->alloc_len);
	g_free (cells);
}


/*
 * BteRowData: A row's data
 */

void
_bte_row_data_init (BteRowData *row)
{
	memset (row, 0, sizeof (*row));
}

void
_bte_row_data_clear (BteRowData *row)
{
	BteCell *cells = row->cells;
	_bte_row_data_init (row);
	row->cells = cells;
}

void
_bte_row_data_fini (BteRowData *row)
{
	if (row->cells)
		_bte_cells_free (_bte_cells_for_cell_array (row->cells));
	row->cells = NULL;
}

static inline gboolean
_bte_row_data_ensure (BteRowData *row, gulong len)
{
	BteCells *cells = _bte_cells_for_cell_array (row->cells);
	if (G_LIKELY (cells && len <= cells->alloc_len))
		return TRUE;

	if (G_UNLIKELY (len >= 0xFFFF))
		return FALSE;

	row->cells = _bte_cells_realloc (cells, len)->cells;

	return TRUE;
}

void
_bte_row_data_insert (BteRowData *row, gulong col, const BteCell *cell)
{
	gulong i;

	if (G_UNLIKELY (!_bte_row_data_ensure (row, row->len + 1)))
		return;

	for (i = row->len; i > col; i--)
		row->cells[i] = row->cells[i - 1];

	row->cells[col] = *cell;
	row->len++;
}

void _bte_row_data_append (BteRowData *row, const BteCell *cell)
{
	if (G_UNLIKELY (!_bte_row_data_ensure (row, row->len + 1)))
		return;

	row->cells[row->len] = *cell;
	row->len++;
}

void _bte_row_data_remove (BteRowData *row, gulong col)
{
	gulong i;

	for (i = col + 1; i < row->len; i++)
		row->cells[i - 1] = row->cells[i];

	if (G_LIKELY (row->len))
		row->len--;
}

void _bte_row_data_fill (BteRowData *row, const BteCell *cell, gulong len)
{
	if (row->len < len) {
		gulong i;

		if (G_UNLIKELY (!_bte_row_data_ensure (row, len)))
			return;

		for (i = row->len; i < len; i++)
			row->cells[i] = *cell;

		row->len = len;
	}
}

void _bte_row_data_shrink (BteRowData *row, gulong max_len)
{
	if (max_len < row->len)
		row->len = max_len;
}

void _bte_row_data_copy (const BteRowData *src, BteRowData *dst)
{
        _bte_row_data_ensure (dst, src->len);
        dst->len = src->len;
        dst->attr = src->attr;
        memcpy(dst->cells, src->cells, src->len * sizeof (src->cells[0]));
}

/* Get the length, ignoring trailing empty cells (with a custom background color). */
guint16 _bte_row_data_nonempty_length (const BteRowData *row)
{
        guint16 len;
        const BteCell *cell;
        for (len = row->len; len > 0; len--) {
                cell = &row->cells[len - 1];
                if (cell->attr.fragment() || cell->c != 0)
                        break;
        }
        return len;
}

