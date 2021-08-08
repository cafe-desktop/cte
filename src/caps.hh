/*
 * Copyright (C) 2001,2002 Red Hat, Inc.
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

#define _BTE_CAP_ESC "\033"		/* Escape */
#define _BTE_CAP_CSI _BTE_CAP_ESC "["	/* Control Sequence Introducer */
#define _BTE_CAP_ST  _BTE_CAP_ESC "\\"	/* String Terminator */
#define _BTE_CAP_OSC _BTE_CAP_ESC "]"	/* Operating System Command */
#define _BTE_CAP_PM  _BTE_CAP_ESC "^"	/* Privacy Message */
#define _BTE_CAP_APC _BTE_CAP_ESC "_"	/* Application Program Command */
#define _BTE_CAP_SS2 _BTE_CAP_ESC "N"	/* Single-shift to G2 */
#define _BTE_CAP_SS3 _BTE_CAP_ESC "O"	/* Single-shift to G3 */
