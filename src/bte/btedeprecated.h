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

#if !defined (__BTE_BTE_H_INSIDE__) && !defined (BTE_COMPILATION)
#error "Only <bte/bte.h> can be included directly."
#endif

#ifndef __BTE_DEPRECATED_H__
#define __BTE_DEPRECATED_H__

#include "bteterminal.h"
#include "btepty.h"
#include "btemacros.h"

#ifndef BTE_DISABLE_DEPRECATION_WARNINGS
#define _BTE_DEPRECATED G_DEPRECATED
#else
#define _BTE_DEPRECATED
#endif

G_BEGIN_DECLS

_BTE_DEPRECATED
_BTE_PUBLIC
int bte_terminal_match_add_gregex(BteTerminal *terminal,
                                  GRegex *gregex,
                                  GRegexMatchFlags gflags) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_match_set_cursor(BteTerminal *terminal,
                                   int tag,
                                   CdkCursor *cursor) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_match_set_cursor_type(BteTerminal *terminal,
					int tag,
                                        CdkCursorType cursor_type) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
char *bte_terminal_match_check(BteTerminal *terminal,
			       glong column, glong row,
			       int *tag) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) G_GNUC_MALLOC;

_BTE_DEPRECATED
_BTE_PUBLIC
gboolean bte_terminal_event_check_gregex_simple(BteTerminal *terminal,
                                                CdkEvent *event,
                                                GRegex **regexes,
                                                gsize n_regexes,
                                                GRegexMatchFlags match_flags,
                                                char **matches) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

_BTE_DEPRECATED
_BTE_PUBLIC
void      bte_terminal_search_set_gregex      (BteTerminal *terminal,
					       GRegex      *gregex,
                                               GRegexMatchFlags gflags) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
GRegex   *bte_terminal_search_get_gregex      (BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
gboolean bte_terminal_spawn_sync(BteTerminal *terminal,
                                 BtePtyFlags pty_flags,
                                 const char *working_directory,
                                 char **argv,
                                 char **envv,
                                 GSpawnFlags spawn_flags,
                                 GSpawnChildSetupFunc child_setup,
                                 gpointer child_setup_data,
                                 GPid *child_pid /* out */,
                                 GCancellable *cancellable,
                                 GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(4);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_pty_close (BtePty *pty) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_copy_clipboard(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_get_geometry_hints(BteTerminal *terminal,
                                     CdkGeometry *hints,
                                     int min_rows,
                                     int min_columns) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_set_geometry_hints_for_window(BteTerminal *terminal,
                                                CtkWindow *window) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

_BTE_DEPRECATED
_BTE_PUBLIC
const char *bte_terminal_get_icon_title(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
gboolean bte_terminal_set_encoding(BteTerminal *terminal,
                                   const char *codeset,
                                   GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
const char *bte_terminal_get_encoding(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
char *bte_terminal_get_text_include_trailing_spaces(BteTerminal *terminal,
						    BteSelectionFunc is_selected,
						    gpointer user_data,
						    GArray *attributes) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) G_GNUC_MALLOC;

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_set_rewrap_on_resize(BteTerminal *terminal,
                                       gboolean rewrap) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);
_BTE_DEPRECATED
_BTE_PUBLIC
gboolean bte_terminal_get_rewrap_on_resize(BteTerminal *terminal) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
void bte_terminal_feed_child_binary(BteTerminal *terminal,
                                    const guint8 *data,
                                    gsize length) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_DEPRECATED
_BTE_PUBLIC
char **bte_get_encodings(gboolean include_aliases) _BTE_CXX_NOEXCEPT;

_BTE_DEPRECATED
_BTE_PUBLIC
gboolean bte_get_encoding_supported(const char *encoding) _BTE_CXX_NOEXCEPT;

G_END_DECLS

#undef _BTE_DEPRECATED

#endif /* !__BTE_DEPRECATED__H__ */
