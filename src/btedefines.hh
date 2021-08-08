/*
 * Copyright (C) 2001-2004 Red Hat, Inc.
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

#pragma once

#define BTE_TAB_WIDTH			8
#define BTE_LINE_WIDTH			1
#define BTE_ROWS			24
#define BTE_COLUMNS			80

/*
 * R8G8B8 colors are encoded in 25 bits as follows:
 *
 * 0 .. 255:
 *   Colors set by SGR 256-color extension (38/48;5;index).
 *   These are direct indices into the color palette.
 *
 * 256 .. BTE_PALETTE_SIZE - 1 (261):
 *   Special values, such as default colors.
 *   These are direct indices into the color palette.
 *
 * BTE_LEGACY_COLORS_OFFSET (512) .. BTE_LEGACY_COLORS_OFFSET + BTE_LEGACY_FULL_COLOR_SET_SIZE - 1 (527):
 *   Colors set by legacy escapes (30..37/40..47, 90..97/100..107).
 *   These are translated to 0 .. 15 before looking up in the palette, taking bold into account.
 *
 * BTE_DIM_COLORS (2^10) .. :
 *   Dimmed version of the above, for foreground colors.
 *   Cell attributes can't have these colors.
 *
 * BTE_RGB_COLOR (2^24) .. BTE_RGB_COLOR + 16Mi - 1 (2^25 - 1):
 *   Colors set by SGR truecolor extension (38/48;2;red;green;blue)
 *   These are direct RGB values.
 *
 * R4G5B4-bit-per-component colours are encoded the same, except for
 * direct colours which are reduced to 13-bit colours and stored as
 * direct values with bit 1 << 13 set.
 */

#define BTE_LEGACY_COLORS_OFFSET	(1U << 9)
#define BTE_LEGACY_COLOR_SET_SIZE	8
#define BTE_LEGACY_FULL_COLOR_SET_SIZE	16
#define BTE_COLOR_PLAIN_OFFSET		0
#define BTE_COLOR_BRIGHT_OFFSET		8
#define BTE_DIM_COLOR                   (1U << 10)
#define BTE_RGB_COLOR_MASK(rb,gb,bb)    (1U << ((rb) + (gb) + (bb)))
#define BTE_RGB_COLOR(bb,gb,rb,r,g,b)   (BTE_RGB_COLOR_MASK(rb,gb,bb) |   \
                                         ((((r) >> (8 - (rb))) & ((1U << (rb)) -  1U)) << ((gb) + (bb))) | \
                                         ((((g) >> (8 - (gb))) & ((1U << (gb)) -  1U)) << (bb)) | \
                                         (((b) >> (8 - (bb))) & ((1U << (bb)) -  1U)))
#define BTE_RGB_COLOR_GET_COMPONENT(packed,shift,bits) \
        ((((packed) >> (shift)) & ((1U << (bits)) - 1U)) << (8 - bits) | ((1U << (8 - bits)) >> 1))

#define BTE_DEFAULT_FG			256
#define BTE_DEFAULT_BG			257
#define BTE_BOLD_FG			258
#define BTE_HIGHLIGHT_FG		259
#define BTE_HIGHLIGHT_BG		260
#define BTE_CURSOR_BG			261
#define BTE_CURSOR_FG                   262
#define BTE_PALETTE_SIZE		263

#define BTE_SCROLLBACK_INIT		512
#define BTE_DEFAULT_CURSOR		CDK_XTERM
#define BTE_MOUSING_CURSOR		CDK_LEFT_PTR
#define BTE_HYPERLINK_CURSOR		CDK_HAND2
#define BTE_HYPERLINK_CURSOR_DEBUG	CDK_SPIDER
#define BTE_CHILD_INPUT_PRIORITY	G_PRIORITY_DEFAULT_IDLE
#define BTE_CHILD_OUTPUT_PRIORITY	G_PRIORITY_HIGH
#define BTE_MAX_INPUT_READ		0x1000
#define BTE_DISPLAY_TIMEOUT		10
#define BTE_UPDATE_TIMEOUT		15
#define BTE_UPDATE_REPEAT_TIMEOUT	30
#define BTE_MAX_PROCESS_TIME		100
#define BTE_CELL_BBOX_SLACK		1
#define BTE_DEFAULT_UTF8_AMBIGUOUS_WIDTH 1

#define BTE_UTF8_BPC                    (4) /* Maximum number of bytes used per UTF-8 character */

/* Keep in decreasing order of precedence. */
#define BTE_COLOR_SOURCE_ESCAPE 0
#define BTE_COLOR_SOURCE_API 1

#define BTE_FONT_SCALE_MIN (.25)
#define BTE_FONT_SCALE_MAX (4.)
#define BTE_CELL_SCALE_MIN (1.)
#define BTE_CELL_SCALE_MAX (2.)

/* Minimum time between two beeps (µs) */
#define BTE_BELL_MINIMUM_TIME_DIFFERENCE (100000)

/* Maximum length of a URI in the OSC 8 escape sequences. There's no de jure limit,
 * 2000-ish the de facto standard, and Internet Explorer supports 2083.
 * See also the comment of BTE_HYPERLINK_TOTAL_LENGTH_MAX. */
#define BTE_HYPERLINK_URI_LENGTH_MAX    2083

/* Maximum number of URIs in the ring for a given screen (as in "normal" vs "alternate" screen)
 * at a time. Idx 0 is a placeholder for no hyperlink, URIs have indexes from 1 to
 * BTE_HYPERLINK_COUNT_MAX inclusive, plus one more technical idx is also required, see below.
 * This is just a safety cap because the number of URIs is bound by the number of cells in the ring
 * (excluding the stream) which should be way lower than this at sane window sizes.
 * Make sure there are enough bits to store them in BteCellAttr.hyperlink_idx.
 * Also make sure _bte_ring_hyperlink_gc() can allocate a large enough bitmap. */
#define BTE_HYPERLINK_COUNT_MAX         ((1 << 20) - 2)

/* Used when thawing a row from the stream in order to display it, to denote
 * hyperlinks whose target is currently irrelevant.
 * Make sure there are enough bits to store this in BteCellAttr.hyperlink_idx */
#define BTE_HYPERLINK_IDX_TARGET_IN_STREAM      (BTE_HYPERLINK_COUNT_MAX + 1)

/* Max length allowed in the id= parameter of an OSC 8 sequence.
 * See also the comment of BTE_HYPERLINK_TOTAL_LENGTH_MAX. */
#define BTE_HYPERLINK_ID_LENGTH_MAX     250

/* Max length of all the hyperlink data stored in the streams as a string.
 * Currently the hyperlink data is the ID and URI and a separator in between.
 * Make sure there are enough bits to store this in BteStreamCellAttr.hyperlink_length */
#define BTE_HYPERLINK_TOTAL_LENGTH_MAX  (BTE_HYPERLINK_ID_LENGTH_MAX + 1 + BTE_HYPERLINK_URI_LENGTH_MAX)

/* Max length of title */
#define BTE_WINDOW_TITLE_MAX_LENGTH (1024)

/* Max depth of title stack */
#define BTE_WINDOW_TITLE_STACK_MAX_DEPTH (8)

/* Maximum length of a paragraph, in lines, that might get proper RingView (BiDi) treatment. */
#define BTE_RINGVIEW_PARAGRAPH_LENGTH_MAX   500

#define BTE_VERSION_NUMERIC ((BTE_MAJOR_VERSION) * 10000 + (BTE_MINOR_VERSION) * 100 + (BTE_MICRO_VERSION))

#define BTE_TERMINFO_NAME "xterm-256color"
