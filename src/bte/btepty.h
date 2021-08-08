/*
 * Copyright © 2009, 2010 Christian Persch
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

#ifndef __BTE_BTE_PTY_H__
#define __BTE_BTE_PTY_H__

#if !defined (__BTE_BTE_H_INSIDE__) && !defined (BTE_COMPILATION)
#error "Only <bte/bte.h> can be included directly."
#endif

#include <gio/gio.h>

#include "bteenums.h"
#include "btemacros.h"

G_BEGIN_DECLS

#define BTE_SPAWN_NO_PARENT_ENVV        (1 << 25)
#define BTE_SPAWN_NO_SYSTEMD_SCOPE      (1 << 26)
#define BTE_SPAWN_REQUIRE_SYSTEMD_SCOPE (1 << 27)

_BTE_PUBLIC
GQuark bte_pty_error_quark (void) _BTE_CXX_NOEXCEPT;

/**
 * BTE_PTY_ERROR:
 *
 * Error domain for BTE PTY errors. Errors in this domain will be from the #BtePtyError
 * enumeration. See #GError for more information on error domains.
 */
#define BTE_PTY_ERROR (bte_pty_error_quark ())

/* BTE PTY object */

#define BTE_TYPE_PTY            (bte_pty_get_type())
#define BTE_PTY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BTE_TYPE_PTY, BtePty))
#define BTE_PTY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  BTE_TYPE_PTY, BtePtyClass))
#define BTE_IS_PTY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BTE_TYPE_PTY))
#define BTE_IS_PTY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  BTE_TYPE_PTY))
#define BTE_PTY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  BTE_TYPE_PTY, BtePtyClass))

typedef struct _BtePty        BtePty;
typedef struct _BtePtyClass   BtePtyClass;

_BTE_PUBLIC
GType bte_pty_get_type (void);

_BTE_PUBLIC
BtePty *bte_pty_new_sync (BtePtyFlags flags,
                          GCancellable *cancellable,
                          GError **error) _BTE_CXX_NOEXCEPT;

_BTE_PUBLIC
BtePty *bte_pty_new_foreign_sync (int fd,
                                  GCancellable *cancellable,
                                  GError **error) _BTE_CXX_NOEXCEPT;

_BTE_PUBLIC
int bte_pty_get_fd (BtePty *pty) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
void bte_pty_child_setup (BtePty *pty) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
gboolean bte_pty_get_size (BtePty *pty,
                           int *rows,
                           int *columns,
                           GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
gboolean bte_pty_set_size (BtePty *pty,
                           int rows,
                           int columns,
                           GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

_BTE_PUBLIC
gboolean bte_pty_set_utf8 (BtePty *pty,
                           gboolean utf8,
                           GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(BtePty, g_object_unref)

_BTE_PUBLIC
void bte_pty_spawn_async(BtePty *pty,
                         const char *working_directory,
                         char **argv,
                         char **envv,
                         GSpawnFlags spawn_flags,
                         GSpawnChildSetupFunc child_setup,
                         gpointer child_setup_data,
                         GDestroyNotify child_setup_data_destroy,
                         int timeout,
                         GCancellable *cancellable,
                         GAsyncReadyCallback callback,
                         gpointer user_data) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(3);

_BTE_PUBLIC
void bte_pty_spawn_with_fds_async(BtePty *pty,
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
                                  GCancellable *cancellable,
                                  GAsyncReadyCallback callback,
                                  gpointer user_data) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(3);

_BTE_PUBLIC
gboolean bte_pty_spawn_finish(BtePty *pty,
                              GAsyncResult *result,
                              GPid *child_pid /* out */,
                              GError **error) _BTE_CXX_NOEXCEPT _BTE_GNUC_NONNULL(1) _BTE_GNUC_NONNULL(2);

G_END_DECLS

#endif /* __BTE_BTE_PTY_H__ */
