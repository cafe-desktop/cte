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

#ifndef __BTE_BTE_TERMINAL_H__
#define __BTE_BTE_TERMINAL_H__

#if !defined (__BTE_BTE_H_INSIDE__) && !defined (BTE_COMPILATION)
#error "Only <bte/bte.h> can be included directly."
#endif

#include <glib.h>
#include <gio/gio.h>
#include <pango/pango.h>
#include <ctk/ctk.h>

#include "bteenums.h"
#include "btemacros.h"
#include "btepty.h"
#include "bteregex.h"

G_BEGIN_DECLS

#define BTE_TYPE_TERMINAL            (bte_terminal_get_type())
#define BTE_TERMINAL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BTE_TYPE_TERMINAL, BteTerminal))
#define BTE_TERMINAL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  BTE_TYPE_TERMINAL, BteTerminalClass))
#define BTE_IS_TERMINAL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BTE_TYPE_TERMINAL))
#define BTE_IS_TERMINAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  BTE_TYPE_TERMINAL))
#define BTE_TERMINAL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  BTE_TYPE_TERMINAL, BteTerminalClass))

typedef struct _BteTerminal             BteTerminal;
typedef struct _BteTerminalClass        BteTerminalClass;
typedef struct _BteTerminalClassPrivate BteTerminalClassPrivate;
typedef struct _BteCharAttributes       BteCharAttributes;

/**
 * BteTerminal:
 */
struct _BteTerminal {
	CtkWidget widget;
        /*< private >*/
	gpointer *_unused_padding[1]; /* FIXMEchpe: remove this field on the next ABI break */
};

/**
 * BteTerminalClass:
 *
 * All of these fields should be considered read-only, except for derived classes.
 */
struct _BteTerminalClass {
	/*< public > */
	/* Inherited parent class. */
	CtkWidgetClass parent_class;

	/*< protected > */
	/* Default signal handlers. */
	void (*eof)(BteTerminal* terminal);
	void (*child_exited)(BteTerminal* terminal, int status);
	void (*encoding_changed)(BteTerminal* terminal);
	void (*char_size_changed)(BteTerminal* terminal, guint char_width, guint char_height);
	void (*window_title_changed)(BteTerminal* terminal);
	void (*icon_title_changed)(BteTerminal* terminal);
	void (*selection_changed)(BteTerminal* terminal);
	void (*contents_changed)(BteTerminal* terminal);
	void (*cursor_moved)(BteTerminal* terminal);
	void (*commit)(BteTerminal* terminal, const gchar *text, guint size);

	void (*deiconify_window)(BteTerminal* terminal);
	void (*iconify_window)(BteTerminal* terminal);
	void (*raise_window)(BteTerminal* terminal);
	void (*lower_window)(BteTerminal* terminal);
	void (*refresh_window)(BteTerminal* terminal);
	void (*restore_window)(BteTerminal* terminal);
	void (*maximize_window)(BteTerminal* terminal);
	void (*resize_window)(BteTerminal* terminal, guint width, guint height);
	void (*move_window)(BteTerminal* terminal, guint x, guint y);

        /* FIXMEchpe: should these return gboolean and have defaul thandlers
         * settings the "scale" property?
         */
	void (*increase_font_size)(BteTerminal* terminal);
	void (*decrease_font_size)(BteTerminal* terminal);

	void (*text_modified)(BteTerminal* terminal);
	void (*text_inserted)(BteTerminal* terminal);
	void (*text_deleted)(BteTerminal* terminal);
	void (*text_scrolled)(BteTerminal* terminal, gint delta);
	void (*copy_clipboard)(BteTerminal* terminal);
	void (*paste_clipboard)(BteTerminal* terminal);

	void (*bell)(BteTerminal* terminal);

        /* Padding for future expansion. */
        gpointer padding[16];

        BteTerminalClassPrivate *priv;
};

/* The structure we return as the supplemental attributes for strings. */
struct _BteCharAttributes {
        /*< private >*/
        long row, column;  /* logical column */
	PangoColor fore, back;
	guint underline:1, strikethrough:1, columns:4;
};

typedef gboolean (*BteSelectionFunc)(BteTerminal *terminal,
                                     glong column,
                                     glong row,
                                     gpointer data) _BTE_GNUC_NONNULL(1);

/* The widget's type. */
_BTE_PUBLIC
GType bte_terminal_get_type(void);

_BTE_PUBLIC
CtkWidget *bte_terminal_new(void) _BTE_CXX_NOEXCEPT;

_BTE_PUBLIC
BtePty *bte_terminal_pty_new_sync (BteTerminal *terminal,
                                   BtePtyFlags flags,
                                   GCancellable *cancellable,
                                   GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_watch_child (BteTerminal *terminal,
                               GPid child_pid) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

typedef void (* BteTerminalSpawnAsyncCallback) (BteTerminal *terminal,
                                                GPid pid,
                                                GError *error,
                                                gpointer user_data);

_BTE_PUBLIC
void bte_terminal_spawn_async(BteTerminal *terminal,
                              BtePtyFlags pty_flags,
                              const char *working_directory,
                              char **argv,
                              char **envv,
                              GSpawnFlags spawn_flags,
                              GSpawnChildSetupFunc child_setup,
                              gpointer child_setup_data,
                              GDestroyNotify child_setup_data_destroy,
                              int timeout,
                              GCancellable *cancellable,
                              BteTerminalSpawnAsyncCallback callback,
                              gpointer user_data) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(4);

_BTE_PUBLIC
void bte_terminal_spawn_with_fds_async(BteTerminal* terminal,
                                       BtePtyFlags pty_flags,
                                       char const* working_directory,
                                       char const* const* argv,
                                       char const* const* envv,
                                       int const* fds,
                                       int n_fds,
                                       int const* map_fds,
                                       int n_map_fds,
                                       GSpawnFlags spawn_flags,
                                       GSpawnChildSetupFunc child_setup,
                                       gpointer child_setup_data,
                                       GDestroyNotify child_setup_data_destroy,
                                       int timeout,
                                       GCancellable* cancellable,
                                       BteTerminalSpawnAsyncCallback callback,
                                       gpointer user_data) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(4);

/* Send data to the terminal to display, or to the terminal's forked command
 * to handle in some way.  If it's 'cat', they should be the same. */
_BTE_PUBLIC
void bte_terminal_feed(BteTerminal *terminal,
                       const char *data,
                       gssize length) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_feed_child(BteTerminal *terminal,
                             const char *text,
                             gssize length) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Copy currently-selected text to the clipboard, or from the clipboard to
 * the terminal. */
_BTE_PUBLIC
void bte_terminal_copy_clipboard_format(BteTerminal *terminal,
                                        BteFormat format) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_paste_clipboard(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_copy_primary(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_paste_primary(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_select_all(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_unselect_all(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* By-word selection */
_BTE_PUBLIC
void bte_terminal_set_word_char_exceptions(BteTerminal *terminal,
                                           const char *exceptions) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
const char *bte_terminal_get_word_char_exceptions(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set the terminal's size. */
_BTE_PUBLIC
void bte_terminal_set_size(BteTerminal *terminal,
			   glong columns, glong rows) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_font_scale(BteTerminal *terminal,
                                 gdouble scale) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gdouble bte_terminal_get_font_scale(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_cell_width_scale(BteTerminal *terminal,
                                       double scale) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
double bte_terminal_get_cell_width_scale(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_cell_height_scale(BteTerminal *terminal,
                                        double scale) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
double bte_terminal_get_cell_height_scale(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set various on-off settings. */
_BTE_PUBLIC
void bte_terminal_set_text_blink_mode(BteTerminal *terminal,
                                      BteTextBlinkMode text_blink_mode) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
BteTextBlinkMode bte_terminal_get_text_blink_mode(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_audible_bell(BteTerminal *terminal,
                                   gboolean is_audible) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_audible_bell(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_scroll_on_output(BteTerminal *terminal,
                                       gboolean scroll) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_scroll_on_output(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_scroll_on_keystroke(BteTerminal *terminal,
					  gboolean scroll) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_scroll_on_keystroke(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set the color scheme. */
_BTE_PUBLIC
void bte_terminal_set_color_bold(BteTerminal *terminal,
                                 const CdkRGBA *bold) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_color_foreground(BteTerminal *terminal,
                                       const CdkRGBA *foreground) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);
_BTE_PUBLIC
void bte_terminal_set_color_background(BteTerminal *terminal,
                                       const CdkRGBA *background) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);
_BTE_PUBLIC
void bte_terminal_set_color_cursor(BteTerminal *terminal,
                                   const CdkRGBA *cursor_background) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_color_cursor_foreground(BteTerminal *terminal,
                                              const CdkRGBA *cursor_foreground) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_color_highlight(BteTerminal *terminal,
                                      const CdkRGBA *highlight_background) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_color_highlight_foreground(BteTerminal *terminal,
                                                 const CdkRGBA *highlight_foreground) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_colors(BteTerminal *terminal,
                             const CdkRGBA *foreground,
                             const CdkRGBA *background,
                             const CdkRGBA *palette,
                             gsize palette_size) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_default_colors(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set whether or not the cursor blinks. */
_BTE_PUBLIC
void bte_terminal_set_cursor_blink_mode(BteTerminal *terminal,
					BteCursorBlinkMode mode) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
BteCursorBlinkMode bte_terminal_get_cursor_blink_mode(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set cursor shape */
_BTE_PUBLIC
void bte_terminal_set_cursor_shape(BteTerminal *terminal,
				   BteCursorShape shape) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
BteCursorShape bte_terminal_get_cursor_shape(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set the number of scrollback lines, above or at an internal minimum. */
_BTE_PUBLIC
void bte_terminal_set_scrollback_lines(BteTerminal *terminal,
                                       glong lines) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
glong bte_terminal_get_scrollback_lines(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set or retrieve the current font. */
_BTE_PUBLIC
void bte_terminal_set_font(BteTerminal *terminal,
			   const PangoFontDescription *font_desc) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
const PangoFontDescription *bte_terminal_get_font(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_allow_bold(BteTerminal *terminal,
                                 gboolean allow_bold) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_allow_bold(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_bold_is_bright(BteTerminal *terminal,
                                     gboolean bold_is_bright) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_bold_is_bright(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_allow_hyperlink(BteTerminal *terminal,
                                      gboolean allow_hyperlink) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_allow_hyperlink(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Check if the terminal is the current selection owner. */
_BTE_PUBLIC
gboolean bte_terminal_get_has_selection(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Set what happens when the user strikes backspace or delete. */
_BTE_PUBLIC
void bte_terminal_set_backspace_binding(BteTerminal *terminal,
					BteEraseBinding binding) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_set_delete_binding(BteTerminal *terminal,
				     BteEraseBinding binding) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* BiDi and shaping */
_BTE_PUBLIC
void bte_terminal_set_enable_bidi(BteTerminal *terminal,
                                  gboolean enable_bidi) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_enable_bidi(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_enable_shaping(BteTerminal *terminal,
                                     gboolean enable_shaping) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_enable_shaping(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Manipulate the autohide setting. */
_BTE_PUBLIC
void bte_terminal_set_mouse_autohide(BteTerminal *terminal,
                                     gboolean setting) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_mouse_autohide(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Reset the terminal, optionally clearing the tab stops and line history. */
_BTE_PUBLIC
void bte_terminal_reset(BteTerminal *terminal,
                        gboolean clear_tabstops,
			gboolean clear_history) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Read the contents of the terminal, using a callback function to determine
 * if a particular location on the screen (0-based) is interesting enough to
 * include.  Each byte in the returned string will have a corresponding
 * BteCharAttributes structure in the passed GArray, if the array was not %NULL.
 * Note that it will have one entry per byte, not per character, so indexes
 * should match up exactly. */
_BTE_PUBLIC
char *bte_terminal_get_text(BteTerminal *terminal,
			    BteSelectionFunc is_selected,
			    gpointer user_data,
			    GArray *attributes) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) G_GNUC_MALLOC;
_BTE_PUBLIC
char *bte_terminal_get_text_range(BteTerminal *terminal,
				  glong start_row, glong start_col,
				  glong end_row, glong end_col,
				  BteSelectionFunc is_selected,
				  gpointer user_data,
				  GArray *attributes) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) G_GNUC_MALLOC;
_BTE_PUBLIC
void bte_terminal_get_cursor_position(BteTerminal *terminal,
				      glong *column,
                                      glong *row) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
char *bte_terminal_hyperlink_check_event(BteTerminal *terminal,
                                         CdkEvent *event) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2) G_GNUC_MALLOC;

/* Add a matching expression, returning the tag the widget assigns to that
 * expression. */
_BTE_PUBLIC
int bte_terminal_match_add_regex(BteTerminal *terminal,
                                 BteRegex *regex,
                                 guint32 flags) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);
/* Set the cursor to be used when the pointer is over a given match. */
_BTE_PUBLIC
void bte_terminal_match_set_cursor_name(BteTerminal *terminal,
					int tag,
                                        const char *cursor_name) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(3);
_BTE_PUBLIC
void bte_terminal_match_remove(BteTerminal *terminal,
                               int tag) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_match_remove_all(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Check if a given cell on the screen contains part of a matched string.  If
 * it does, return the string, and store the match tag in the optional tag
 * argument. */
_BTE_PUBLIC
char *bte_terminal_match_check_event(BteTerminal *terminal,
                                     CdkEvent *event,
                                     int *tag) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2) G_GNUC_MALLOC;
_BTE_PUBLIC
char **bte_terminal_event_check_regex_array(BteTerminal *terminal,
                                            CdkEvent *event,
                                            BteRegex **regexes,
                                            gsize n_regexes,
                                            guint32 match_flags,
                                            gsize *n_matches) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2) G_GNUC_MALLOC;
_BTE_PUBLIC
gboolean bte_terminal_event_check_regex_simple(BteTerminal *terminal,
                                               CdkEvent *event,
                                               BteRegex **regexes,
                                               gsize n_regexes,
                                               guint32 match_flags,
                                               char **matches) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

_BTE_PUBLIC
void      bte_terminal_search_set_regex      (BteTerminal *terminal,
                                              BteRegex    *regex,
                                              guint32      flags) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
BteRegex *bte_terminal_search_get_regex      (BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void      bte_terminal_search_set_wrap_around (BteTerminal *terminal,
					       gboolean     wrap_around) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean  bte_terminal_search_get_wrap_around (BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean  bte_terminal_search_find_previous   (BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean  bte_terminal_search_find_next       (BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);


/* CJK compatibility setting */
_BTE_PUBLIC
void bte_terminal_set_cjk_ambiguous_width(BteTerminal *terminal,
                                          int width) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
int bte_terminal_get_cjk_ambiguous_width(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_terminal_set_pty(BteTerminal *terminal,
                          BtePty *pty) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
BtePty *bte_terminal_get_pty(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* Accessors for bindings. */
_BTE_PUBLIC
glong bte_terminal_get_char_width(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
glong bte_terminal_get_char_height(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
glong bte_terminal_get_row_count(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
glong bte_terminal_get_column_count(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
const char *bte_terminal_get_window_title(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
const char *bte_terminal_get_icon_title(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
const char *bte_terminal_get_current_directory_uri(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
const char *bte_terminal_get_current_file_uri(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* misc */
_BTE_PUBLIC
void bte_terminal_set_input_enabled (BteTerminal *terminal,
                                     gboolean enabled) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
gboolean bte_terminal_get_input_enabled (BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

/* rarely useful functions */
_BTE_PUBLIC
void bte_terminal_set_clear_background(BteTerminal* terminal,
                                       gboolean setting) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_PUBLIC
void bte_terminal_get_color_background_for_draw(BteTerminal* terminal,
                                                CdkRGBA* color) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

/* Writing contents out */
_BTE_PUBLIC
gboolean bte_terminal_write_contents_sync (BteTerminal *terminal,
                                           GOutputStream *stream,
                                           BteWriteFlags flags,
                                           GCancellable *cancellable,
                                           GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

/* Images */

/* Set or get whether SIXEL image support is enabled */
_BTE_PUBLIC
void bte_terminal_set_enable_sixel(BteTerminal *terminal,
                                    gboolean enabled) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
gboolean bte_terminal_get_enable_sixel(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(BteTerminal, g_object_unref)

G_END_DECLS

#endif /* __BTE_BTE_TERMINAL_H__ */
