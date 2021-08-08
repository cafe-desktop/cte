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

#ifndef bte_bteaccess_h_included
#define bte_bteaccess_h_included


#include <glib.h>
#include <ctk/ctk.h>
#include <ctk/ctk-a11y.h>

G_BEGIN_DECLS

#define BTE_TYPE_TERMINAL_ACCESSIBLE            (_bte_terminal_accessible_get_type ())
#define BTE_TERMINAL_ACCESSIBLE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), BTE_TYPE_TERMINAL_ACCESSIBLE, BteTerminalAccessible))
#define BTE_TERMINAL_ACCESSIBLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BTE_TYPE_TERMINAL_ACCESSIBLE, BteTerminalAccessibleClass))
#define BTE_IS_TERMINAL_ACCESSIBLE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), BTE_TYPE_TERMINAL_ACCESSIBLE))
#define BTE_IS_TERMINAL_ACCESSIBLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BTE_TYPE_TERMINAL_ACCESSIBLE))
#define BTE_TERMINAL_ACCESSIBLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BTE_TYPE_TERMINAL_ACCESSIBLE, BteTerminalAccessibleClass))

typedef struct _BteTerminalAccessible      BteTerminalAccessible;
typedef struct _BteTerminalAccessibleClass BteTerminalAccessibleClass;

/**
 * BteTerminalAccessible:
 *
 * The accessible peer for #BteTerminal.
 */
struct _BteTerminalAccessible {
	CtkWidgetAccessible parent;
};

struct _BteTerminalAccessibleClass {
	CtkWidgetAccessibleClass parent_class;
};

GType _bte_terminal_accessible_get_type(void);

G_END_DECLS

#endif
