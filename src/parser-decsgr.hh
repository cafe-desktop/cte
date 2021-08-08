/*
 * Copyright © 2018 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License) or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful)
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not) see <https://www.gnu.org/licenses/>.
 */

#if !defined(_BTE_SGR) || !defined(_BTE_NGR)
#error "Must define _BTE_SGR and _BTE_NGR before including this file"
#endif

#define SGR(set, name, value) _BTE_SGR(set##_##name, value)
#define NGR(set, name, value) _BTE_NGR(set##_##name, value)

NGR(RESET, ALL, 0)
NGR(SET, SUPERSCRIPT, 4)
NGR(SET, SUBSCRIPT, 5)
NGR(SET, OVERLINE, 6)
NGR(SET, TRANSPARENCY, 8)
NGR(RESET, SUPERSUBSCRIPT, 24)
NGR(RESET, OVERLINE, 26)
NGR(RESET, TRANSPARENCY, 28)

#undef SGR
#undef NGR
