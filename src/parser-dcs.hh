/*
 * Copyright © 2015 David Herrmann <dh.herrmann@gmail.com>
 * Copyright © 2018 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined(_BTE_SEQ) || !defined(_BTE_NOQ)
#error "Must define _BTE_SEQ and _BTE_NOQ before including this file"
#endif

_BTE_NOQ(RLOGIN_MML,             DCS,    'm',  NONE,  1, HASH     ) /* RLogin music macro language */
_BTE_NOQ(DECREGIS,               DCS,    'p',  NONE,  0, NONE     ) /* ReGIS-graphics */
_BTE_NOQ(DECRSTS,                DCS,    'p',  NONE,  1, CASH     ) /* restore-terminal-state */
_BTE_NOQ(XTERM_STCAP,            DCS,    'p',  NONE,  1, PLUS     ) /* xterm set termcap/terminfo */
_BTE_NOQ(DECSIXEL,               DCS,    'q',  NONE,  0, NONE     ) /* SIXEL-graphics */
_BTE_SEQ(DECRQSS,                DCS,    'q',  NONE,  1, CASH     ) /* request-selection-or-setting */
_BTE_NOQ(XTERM_RQTCAP,           DCS,    'q',  NONE,  1, PLUS     ) /* xterm request termcap/terminfo */
_BTE_NOQ(DECLBAN,                DCS,    'r',  NONE,  0, NONE     ) /* load-banner-message */
_BTE_SEQ(DECRQTSR,               DCS,    's',  NONE,  1, CASH     ) /* request-terminal-state-report */
_BTE_SEQ(XDGSYNC,                DCS,    's',  EQUAL, 0, NONE     ) /* synchronous update */
_BTE_NOQ(DECRSPS,                DCS,    't',  NONE,  1, CASH     ) /* restore-presentation-state */
_BTE_NOQ(DECAUPSS,               DCS,    'u',  NONE,  1, BANG     ) /* assign-user-preferred-supplemental-sets */
_BTE_NOQ(DECLANS,                DCS,    'v',  NONE,  0, NONE     ) /* load-answerback-message */
_BTE_NOQ(DECLBD,                 DCS,    'w',  NONE,  0, NONE     ) /* locator-button-define */
_BTE_NOQ(DECPFK,                 DCS,    'x',  NONE,  1, DQUOTE   ) /* program-function-key */
_BTE_NOQ(DECPAK,                 DCS,    'y',  NONE,  1, DQUOTE   ) /* program-alphanumeric-key */
_BTE_NOQ(DECDMAC,                DCS,    'z',  NONE,  1, BANG     ) /* define-macro */
_BTE_NOQ(DECCKD,                 DCS,    'z',  NONE,  1, DQUOTE   ) /* copy-key-default */
_BTE_NOQ(DECDLD,                 DCS,    '{',  NONE,  0, NONE     ) /* dynamically-redefinable-character-sets-extension */
_BTE_NOQ(DECSTUI,                DCS,    '{',  NONE,  1, BANG     ) /* set-terminal-unit-id */
_BTE_NOQ(DECUDK,                 DCS,    '|',  NONE,  0, NONE     ) /* user-defined-keys */
_BTE_NOQ(WYLSFNT,                DCS,    '}',  NONE,  0, NONE     ) /* load soft font */
_BTE_NOQ(DECRPFK,                DCS,    '}',  NONE,  1, DQUOTE   ) /* report function key definition */
_BTE_NOQ(DECRPAK,                DCS,    '~',  NONE,  1, DQUOTE   ) /* report all modifier/alphanumeric key state */
