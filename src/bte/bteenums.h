/*
 * Copyright (C) 2001,2002,2003,2009,2010 Red Hat, Inc.
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

#ifndef __BTE_BTE_ENUMS_H__
#define __BTE_BTE_ENUMS_H__

#if !defined (__BTE_BTE_H_INSIDE__) && !defined (BTE_COMPILATION)
#error "Only <bte/bte.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

/**
 * BteCursorBlinkMode:
 * @BTE_CURSOR_BLINK_SYSTEM: Follow CTK+ settings for cursor blinking.
 * @BTE_CURSOR_BLINK_ON: Cursor blinks.
 * @BTE_CURSOR_BLINK_OFF: Cursor does not blink.
 *
 * An enumerated type which can be used to indicate the cursor blink mode
 * for the terminal.
 */
typedef enum {
        BTE_CURSOR_BLINK_SYSTEM,
        BTE_CURSOR_BLINK_ON,
        BTE_CURSOR_BLINK_OFF
} BteCursorBlinkMode;

/**
 * BteCursorShape:
 * @BTE_CURSOR_SHAPE_BLOCK: Draw a block cursor.  This is the default.
 * @BTE_CURSOR_SHAPE_IBEAM: Draw a vertical bar on the left side of character.
 * This is similar to the default cursor for other CTK+ widgets.
 * @BTE_CURSOR_SHAPE_UNDERLINE: Draw a horizontal bar below the character.
 *
 * An enumerated type which can be used to indicate what should the terminal
 * draw at the cursor position.
 */
typedef enum {
        BTE_CURSOR_SHAPE_BLOCK,
        BTE_CURSOR_SHAPE_IBEAM,
        BTE_CURSOR_SHAPE_UNDERLINE
} BteCursorShape;

/**
 * BteTextBlinkMode:
 * @BTE_TEXT_BLINK_NEVER: Do not blink the text.
 * @BTE_TEXT_BLINK_FOCUSED: Allow blinking text only if the terminal is focused.
 * @BTE_TEXT_BLINK_UNFOCUSED: Allow blinking text only if the terminal is unfocused.
 * @BTE_TEXT_BLINK_ALWAYS: Allow blinking text. This is the default.
 *
 * An enumerated type which can be used to indicate whether the terminal allows
 * the text contents to be blinked.
 *
 * Since: 0.52
 */
typedef enum {
        BTE_TEXT_BLINK_NEVER     = 0,
        BTE_TEXT_BLINK_FOCUSED   = 1,
        BTE_TEXT_BLINK_UNFOCUSED = 2,
        BTE_TEXT_BLINK_ALWAYS    = 3
} BteTextBlinkMode;

/**
 * BteEraseBinding:
 * @BTE_ERASE_AUTO: For backspace, attempt to determine the right value from the terminal's IO settings.  For delete, use the control sequence.
 * @BTE_ERASE_ASCII_BACKSPACE: Send an ASCII backspace character (0x08).
 * @BTE_ERASE_ASCII_DELETE: Send an ASCII delete character (0x7F).
 * @BTE_ERASE_DELETE_SEQUENCE: Send the "@@7" control sequence.
 * @BTE_ERASE_TTY: Send terminal's "erase" setting.
 *
 * An enumerated type which can be used to indicate which string the terminal
 * should send to an application when the user presses the Delete or Backspace
 * keys.
 */
typedef enum {
	BTE_ERASE_AUTO,
	BTE_ERASE_ASCII_BACKSPACE,
	BTE_ERASE_ASCII_DELETE,
	BTE_ERASE_DELETE_SEQUENCE,
	BTE_ERASE_TTY
} BteEraseBinding;

/**
 * BtePtyError:
 * @BTE_PTY_ERROR_PTY_HELPER_FAILED: Obsolete. Deprecated: 0.42
 * @BTE_PTY_ERROR_PTY98_FAILED: failure when using PTY98 to allocate the PTY
 */
typedef enum {
  BTE_PTY_ERROR_PTY_HELPER_FAILED = 0,
  BTE_PTY_ERROR_PTY98_FAILED
} BtePtyError;

/**
 * BtePtyFlags:
 * @BTE_PTY_NO_LASTLOG: Unused. Deprecated: 0.38
 * @BTE_PTY_NO_UTMP: Unused. Deprecated: 0.38
 * @BTE_PTY_NO_WTMP: Unused. Deprecated: 0.38
 * @BTE_PTY_NO_HELPER: Unused. Deprecated: 0.38
 * @BTE_PTY_NO_FALLBACK: Unused. Deprecated: 0.38
 * @BTE_PTY_NO_SESSION: Do not start a new session for the child in
 *   bte_pty_child_setup(). See man:setsid(2) for more information. Since: 0.58
 * @BTE_PTY_NO_CTTY: Do not set the PTY as the controlling TTY for the child
 *   in bte_pty_child_setup(). See man:tty_ioctl(4) for more information. Since: 0.58
 * @BTE_PTY_DEFAULT: the default flags
 */
typedef enum {
  BTE_PTY_NO_LASTLOG  = 1 << 0,
  BTE_PTY_NO_UTMP     = 1 << 1,
  BTE_PTY_NO_WTMP     = 1 << 2,
  BTE_PTY_NO_HELPER   = 1 << 3,
  BTE_PTY_NO_FALLBACK = 1 << 4,
  BTE_PTY_NO_SESSION  = 1 << 5,
  BTE_PTY_NO_CTTY     = 1 << 6,
  BTE_PTY_DEFAULT     = 0
} BtePtyFlags;

/**
 * BteWriteFlags:
 * @BTE_WRITE_DEFAULT: Write contents as UTF-8 text.  This is the default.
 *
 * A flag type to determine how terminal contents should be written
 * to an output stream.
 */
typedef enum {
  BTE_WRITE_DEFAULT = 0
} BteWriteFlags;

/**
 * BteRegexError:
 * @BTE_REGEX_ERROR_INCOMPATIBLE: The PCRE2 library was built without
 *   Unicode support which is required for BTE
 * @BTE_REGEX_ERROR_NOT_SUPPORTED: Regexes are not supported because BTE was
 *   built without PCRE2 support
 *
 * An enum type for regex errors. In addition to the values listed above,
 * any PCRE2 error values may occur.
 *
 * Since: 0.46
 */
typedef enum {
        /* Negative values are PCRE2 errors */

        /* BTE specific values */
        BTE_REGEX_ERROR_INCOMPATIBLE  = G_MAXINT-1,
        BTE_REGEX_ERROR_NOT_SUPPORTED = G_MAXINT
} BteRegexError;

/**
 * BteFormat:
 * @BTE_FORMAT_TEXT: Export as plain text
 * @BTE_FORMAT_HTML: Export as HTML formatted text
 *
 * An enumeration type that can be used to specify the format the selection
 * should be copied to the clipboard in.
 *
 * Since: 0.50
 */
typedef enum {
        BTE_FORMAT_TEXT = 1,
        BTE_FORMAT_HTML = 2
} BteFormat;

/**
 * BteFeatureFlags:
 * @BTE_FEATURE_FLAG_BIDI: whether BTE was built with bidirectional text support
 * @BTE_FEATURE_FLAG_ICU: whether BTE was built with ICU support
 * @BTE_FEATURE_FLAG_SYSTEMD: whether BTE was built with systemd support
 * @BTE_FEATURE_FLAG_SIXEL: whether BTE was built with SIXEL support
 *
 * An enumeration type for features.
 *
 * Since: 0.62
 */
typedef enum /*< skip >*/ {
        BTE_FEATURE_FLAG_BIDI    = 1ULL << 0,
        BTE_FEATURE_FLAG_ICU     = 1ULL << 1,
        BTE_FEATURE_FLAG_SYSTEMD = 1ULL << 2,
        BTE_FEATURE_FLAG_SIXEL   = 1ULL << 3,
        BTE_FEATURE_FLAGS_MASK   = 0xFFFFFFFFFFFFFFFFULL, /* force enum to 64 bit */
} BteFeatureFlags;

G_END_DECLS

#endif /* __BTE_BTE_ENUMS_H__ */
