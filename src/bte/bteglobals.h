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

#ifndef __BTE_BTE_GLOBALS_H__
#define __BTE_BTE_GLOBALS_H__

#include <glib.h>

#include "btemacros.h"
#include "bteenums.h"

G_BEGIN_DECLS

_BTE_PUBLIC
char *bte_get_user_shell(void) _BTE_CXX_NOEXCEPT;

_BTE_PUBLIC
const char *bte_get_features (void) _BTE_CXX_NOEXCEPT;

_BTE_PUBLIC
BteFeatureFlags bte_get_feature_flags(void) _BTE_CXX_NOEXCEPT;

#define BTE_TEST_FLAGS_NONE (G_GUINT64_CONSTANT(0))
#define BTE_TEST_FLAGS_ALL (~G_GUINT64_CONSTANT(0))

_BTE_PUBLIC
void bte_set_test_flags(guint64 flags) _BTE_CXX_NOEXCEPT;

G_END_DECLS

#endif /* __BTE_BTE_GLOBALS_H__ */
