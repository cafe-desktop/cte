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

_BTE_NOQ(DECDHL_TH,              ESCAPE, '3',  NONE,  1, HASH     ) /* double-width-double-height-line: top half */
_BTE_NOQ(DECDHL_BH,              ESCAPE, '4',  NONE,  1, HASH     ) /* double-width-double-height-line: bottom half */
_BTE_NOQ(DECSWL,                 ESCAPE, '5',  NONE,  1, HASH     ) /* single-width-single-height-line */
_BTE_SEQ(DECBI,                  ESCAPE, '6',  NONE,  0, NONE     ) /* back-index */
_BTE_NOQ(DECDWL,                 ESCAPE, '6',  NONE,  1, HASH     ) /* double-width-single-height-line */
_BTE_SEQ(DECSC,                  ESCAPE, '7',  NONE,  0, NONE     ) /* save-cursor */
_BTE_SEQ(DECRC,                  ESCAPE, '8',  NONE,  0, NONE     ) /* restore-cursor */
_BTE_SEQ(DECALN,                 ESCAPE, '8',  NONE,  1, HASH     ) /* screen-alignment-pattern */
_BTE_NOQ(DECFI,                  ESCAPE, '9',  NONE,  0, NONE     ) /* forward-index */
_BTE_NOQ(WYDHL_TH,               ESCAPE, ':',  NONE,  0, HASH     ) /* single width double height line: top half */
_BTE_NOQ(WYDHL_BH,               ESCAPE, ';',  NONE,  0, HASH     ) /* single width double height line: bottom half */
_BTE_NOQ(DECANM,                 ESCAPE, '<',  NONE,  0, NONE     ) /* ansi-mode */
_BTE_SEQ(DECKPAM,                ESCAPE, '=',  NONE,  0, NONE     ) /* keypad-application-mode */
_BTE_SEQ(DECKPNM,                ESCAPE, '>',  NONE,  0, NONE     ) /* keypad-numeric-mode */
_BTE_NOQ(BPH,                    ESCAPE, 'B',  NONE,  0, NONE     ) /* break permitted here */
_BTE_NOQ(NBH,                    ESCAPE, 'C',  NONE,  0, NONE     ) /* no break permitted here */
_BTE_SEQ(IND,                    ESCAPE, 'D',  NONE,  0, NONE     ) /* index */
_BTE_SEQ(NEL,                    ESCAPE, 'E',  NONE,  0, NONE     ) /* next-line */
_BTE_NOQ(SSA,                    ESCAPE, 'F',  NONE,  0, NONE     ) /* start of selected area */
_BTE_NOQ(ESA,                    ESCAPE, 'G',  NONE,  0, NONE     ) /* end of selected area */
_BTE_SEQ(HTS,                    ESCAPE, 'H',  NONE,  0, NONE     ) /* horizontal-tab-set */
_BTE_SEQ(HTJ,                    ESCAPE, 'I',  NONE,  0, NONE     ) /* character tabulation with justification */
_BTE_NOQ(VTS,                    ESCAPE, 'J',  NONE,  0, NONE     ) /* line tabulation set */
_BTE_NOQ(PLD,                    ESCAPE, 'K',  NONE,  0, NONE     ) /* partial line forward */
_BTE_NOQ(PLU,                    ESCAPE, 'L',  NONE,  0, NONE     ) /* partial line backward */
_BTE_SEQ(RI,                     ESCAPE, 'M',  NONE,  0, NONE     ) /* reverse-index */
_BTE_SEQ(SS2,                    ESCAPE, 'N',  NONE,  0, NONE     ) /* single-shift-2 */
_BTE_SEQ(SS3,                    ESCAPE, 'O',  NONE,  0, NONE     ) /* single-shift-3 */
_BTE_NOQ(PU1,                    ESCAPE, 'Q',  NONE,  0, NONE     ) /* private use 1 */
_BTE_NOQ(PU2,                    ESCAPE, 'R',  NONE,  0, NONE     ) /* private use 2 */
_BTE_NOQ(STS,                    ESCAPE, 'S',  NONE,  0, NONE     ) /* set transmit state */
_BTE_NOQ(CCH,                    ESCAPE, 'T',  NONE,  0, NONE     ) /* cancel character */
_BTE_NOQ(MW,                     ESCAPE, 'U',  NONE,  0, NONE     ) /* message waiting */
_BTE_NOQ(SPA,                    ESCAPE, 'V',  NONE,  0, NONE     ) /* start-of-protected-area */
_BTE_NOQ(EPA,                    ESCAPE, 'W',  NONE,  0, NONE     ) /* end-of-guarded-area */
_BTE_NOQ(ST,                     ESCAPE, '\\', NONE,  0, NONE     ) /* string-terminator */
_BTE_NOQ(DMI,                    ESCAPE, '`',  NONE,  0, NONE     ) /* disable manual input */
_BTE_NOQ(INT,                    ESCAPE, 'a',  NONE,  0, NONE     ) /* interrupt */
_BTE_NOQ(EMI,                    ESCAPE, 'b',  NONE,  0, NONE     ) /* enable manual input */
_BTE_SEQ(RIS,                    ESCAPE, 'c',  NONE,  0, NONE     ) /* reset-to-initial-state */
_BTE_NOQ(CMD,                    ESCAPE, 'd',  NONE,  0, NONE     ) /* coding-method-delimiter */
_BTE_NOQ(XTERM_MLHP,             ESCAPE, 'l',  NONE,  0, NONE     ) /* xterm-memory-lock-hp-bugfix */
_BTE_NOQ(XTERM_MUHP,             ESCAPE, 'm',  NONE,  0, NONE     ) /* xterm-memory-unlock-hp-bugfix */
_BTE_SEQ(LS2,                    ESCAPE, 'n',  NONE,  0, NONE     ) /* locking-shift-2 */
_BTE_SEQ(LS3,                    ESCAPE, 'o',  NONE,  0, NONE     ) /* locking-shift-3 */
_BTE_SEQ(LS3R,                   ESCAPE, '|',  NONE,  0, NONE     ) /* locking-shift-3-right */
_BTE_SEQ(LS2R,                   ESCAPE, '}',  NONE,  0, NONE     ) /* locking-shift-2-right */
_BTE_SEQ(LS1R,                   ESCAPE, '~',  NONE,  0, NONE     ) /* locking-shift-1-right */
