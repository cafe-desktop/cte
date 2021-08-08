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

_BTE_SEQ(ICH,                    CSI,    '@',  NONE,  0, NONE     ) /* insert-character */
_BTE_NOQ(SL,                     CSI,    '@',  NONE,  1, SPACE    ) /* scroll left */
_BTE_SEQ(CUU,                    CSI,    'A',  NONE,  0, NONE     ) /* cursor-up */
_BTE_NOQ(SR,                     CSI,    'A',  NONE,  1, SPACE    ) /* scroll right */
_BTE_SEQ(CUD,                    CSI,    'B',  NONE,  0, NONE     ) /* cursor-down */
_BTE_NOQ(GSM,                    CSI,    'B',  NONE,  1, SPACE    ) /* graphic size modification */
_BTE_SEQ(CUF,                    CSI,    'C',  NONE,  0, NONE     ) /* cursor-forward */
_BTE_NOQ(GSS,                    CSI,    'C',  NONE,  1, SPACE    ) /* graphic size selection */
_BTE_SEQ(CUB,                    CSI,    'D',  NONE,  0, NONE     ) /* cursor-backward */
_BTE_NOQ(FNT,                    CSI,    'D',  NONE,  1, SPACE    ) /* font selection */
_BTE_SEQ(CNL,                    CSI,    'E',  NONE,  0, NONE     ) /* cursor-next-line */
_BTE_SEQ(CPL,                    CSI,    'F',  NONE,  0, NONE     ) /* cursor-previous-line */
_BTE_NOQ(JFY,                    CSI,    'F',  NONE,  1, SPACE    ) /* justify */
_BTE_NOQ(TSS,                    CSI,    'E',  NONE,  1, SPACE    ) /* thine space specification */
_BTE_SEQ(CHA,                    CSI,    'G',  NONE,  0, NONE     ) /* cursor-horizontal-absolute */
_BTE_NOQ(SPI,                    CSI,    'G',  NONE,  1, SPACE    ) /* spacing increment */
_BTE_SEQ(CUP,                    CSI,    'H',  NONE,  0, NONE     ) /* cursor-position */
_BTE_NOQ(QUAD,                   CSI,    'H',  NONE,  1, SPACE    ) /* quad */
_BTE_SEQ(CHT,                    CSI,    'I',  NONE,  0, NONE     ) /* cursor-horizontal-forward-tabulation */
_BTE_NOQ(SSU,                    CSI,    'I',  NONE,  1, SPACE    ) /* set size unit */
_BTE_SEQ(ED,                     CSI,    'J',  NONE,  0, NONE     ) /* erase-in-display */
_BTE_NOQ(PFS,                    CSI,    'J',  NONE,  1, SPACE    ) /* page format selection */
_BTE_SEQ(DECSED,                 CSI,    'J',  WHAT,  0, NONE     ) /* selective-erase-in-display */
_BTE_SEQ(EL,                     CSI,    'K',  NONE,  0, NONE     ) /* erase-in-line */
_BTE_NOQ(SHS,                    CSI,    'K',  NONE,  1, SPACE    ) /* select character spacing */
_BTE_SEQ(DECSEL,                 CSI,    'K',  WHAT,  0, NONE     ) /* selective-erase-in-line */
_BTE_SEQ(IL,                     CSI,    'L',  NONE,  0, NONE     ) /* insert-line */
_BTE_NOQ(SVS,                    CSI,    'L',  NONE,  1, SPACE    ) /* select line spacing */
_BTE_SEQ(DL,                     CSI,    'M',  NONE,  0, NONE     ) /* delete-line */
_BTE_NOQ(IGS,                    CSI,    'M',  NONE,  1, SPACE    ) /* identify graphic subrepertoire */
_BTE_NOQ(EF,                     CSI,    'N',  NONE,  0, NONE     ) /* erase in field */
_BTE_NOQ(EA,                     CSI,    'O',  NONE,  0, NONE     ) /* erase in area */
_BTE_NOQ(IDCS,                   CSI,    'O',  NONE,  1, SPACE    ) /* identify DCS */
_BTE_SEQ(DCH,                    CSI,    'P',  NONE,  0, NONE     ) /* delete-character */
_BTE_NOQ(PPA,                    CSI,    'P',  NONE,  1, SPACE    ) /* page-position-absolute */
_BTE_NOQ(SEE,                    CSI,    'Q',  NONE,  0, NONE     ) /* select editing extent */
_BTE_NOQ(PPR,                    CSI,    'Q',  NONE,  1, SPACE    ) /* page-position-relative */
_BTE_NOQ(PPB,                    CSI,    'R',  NONE,  1, SPACE    ) /* page-position-backward */
_BTE_SEQ(SU,                     CSI,    'S',  NONE,  0, NONE     ) /* scroll-up */
_BTE_SEQ(SPD,                    CSI,    'S',  NONE,  1, SPACE    ) /* select presentation directions */
_BTE_NOQ(XTERM_SGFX,             CSI,    'S',  WHAT,  0, NONE     ) /* xterm-sixel-graphics */
_BTE_SEQ(SD_OR_XTERM_IHMT,       CSI,    'T',  NONE,  0, NONE     ) /* scroll-down or xterm-initiate-highlight-mouse-tracking */
_BTE_NOQ(DTA,                    CSI,    'T',  NONE,  1, SPACE    ) /* dimension text area */
_BTE_NOQ(XTERM_RTM,              CSI,    'T',  GT,    0, NONE     ) /* xterm-reset-title-mode */
_BTE_NOQ(NP,                     CSI,    'U',  NONE,  0, NONE     ) /* next-page */
_BTE_NOQ(SLH,                    CSI,    'U',  NONE,  1, SPACE    ) /* set line home */
_BTE_NOQ(PP,                     CSI,    'V',  NONE,  0, NONE     ) /* preceding-page */
_BTE_NOQ(SLL,                    CSI,    'V',  NONE,  1, SPACE    ) /* set line limit */
_BTE_SEQ(CTC,                    CSI,    'W',  NONE,  0, NONE     ) /* cursor tabulation control */
_BTE_NOQ(FNK,                    CSI,    'W',  NONE,  1, SPACE    ) /* function key */
_BTE_SEQ(DECST8C,                CSI,    'W',  WHAT,  0, NONE     ) /* set-tab-at-every-8-columns */
_BTE_SEQ(ECH,                    CSI,    'X',  NONE,  0, NONE     ) /* erase-character */
_BTE_NOQ(SPQR,                   CSI,    'X',  NONE,  1, SPACE    ) /* select print quality and rapidity */
_BTE_NOQ(CVT,                    CSI,    'Y',  NONE,  0, NONE     ) /* cursor line tabulation */
_BTE_NOQ(SEF,                    CSI,    'Y',  NONE,  1, SPACE    ) /* sheet eject and feed */
_BTE_SEQ(CBT,                    CSI,    'Z',  NONE,  0, NONE     ) /* cursor-backward-tabulation */
_BTE_NOQ(PEC,                    CSI,    'Z',  NONE, 1, SPACE     ) /* presentation expand or contract */
_BTE_NOQ(SRS,                    CSI,    '[',  NONE,  0, NONE     ) /* start reversed string */
_BTE_NOQ(SSW,                    CSI,    '[',  NONE,  1, SPACE    ) /* set space width */
_BTE_NOQ(PTX,                    CSI,    '\\', NONE,  0, NONE     ) /* parallel texts */
_BTE_NOQ(SACS,                   CSI,    '\\', NONE,  1, SPACE    ) /* set additional character separation */
_BTE_NOQ(SDS,                    CSI,    ']',  NONE,  0, NONE     ) /* start directed string */
_BTE_NOQ(SAPV,                   CSI,    ']',  NONE,  1, SPACE    ) /* select alternative presentation variants */
_BTE_NOQ(SIMD,                   CSI,    '^',  NONE,  0, NONE     ) /* select implicit movement direction */
_BTE_NOQ(STAB,                   CSI,    '^',  NONE,  1, SPACE    ) /* selective tabulation */
_BTE_NOQ(GCC,                    CSI,    '_',  NONE,  1, SPACE    ) /* graphic character combination */
_BTE_SEQ(HPA,                    CSI,    '`',  NONE,  0, NONE     ) /* horizontal-position-absolute */
_BTE_NOQ(TATE,                   CSI,    '`',  NONE,  1, SPACE    ) /* tabulation-aligned-trailing-edge */
_BTE_SEQ(HPR,                    CSI,    'a',  NONE,  0, NONE     ) /* horizontal-position-relative */
_BTE_NOQ(TALE,                   CSI,    'a',  NONE,  1, SPACE    ) /* tabulation-aligned-leading-edge */
_BTE_SEQ(REP,                    CSI,    'b',  NONE,  0, NONE     ) /* repeat */
_BTE_NOQ(TAC,                    CSI,    'b',  NONE,  1, SPACE    ) /* tabulation-aligned-centre */
_BTE_SEQ(DA1,                    CSI,    'c',  NONE,  0, NONE     ) /* primary-device-attributes */
_BTE_SEQ(TCC,                    CSI,    'c',  NONE,  1, SPACE    ) /* tabulation-centred-on-character */
_BTE_SEQ(DA3,                    CSI,    'c',  EQUAL, 0, NONE     ) /* tertiary-device-attributes */
_BTE_SEQ(DA2,                    CSI,    'c',  GT,    0, NONE     ) /* secondary-device-attributes */
_BTE_SEQ(VPA,                    CSI,    'd',  NONE,  0, NONE     ) /* vertical-line-position-absolute */
_BTE_SEQ(TSR,                    CSI,    'd',  NONE,  1, SPACE    ) /* tabulation-stop-remove */
_BTE_SEQ(VPR,                    CSI,    'e',  NONE,  0, NONE     ) /* vertical-line-position-relative */
_BTE_NOQ(SCO,                    CSI,    'e',  NONE,  1, SPACE    ) /* select character orientation */
_BTE_SEQ(HVP,                    CSI,    'f',  NONE,  0, NONE     ) /* horizontal-and-vertical-position */
_BTE_NOQ(SRCS,                   CSI,    'f',  NONE,  1, SPACE    ) /* set reduced character separation */
_BTE_SEQ(TBC,                    CSI,    'g',  NONE,  0, NONE     ) /* tab-clear */
_BTE_NOQ(SCS,                    CSI,    'g',  NONE,  1, SPACE    ) /* set character spacing */
_BTE_NOQ(DECLFKC,                CSI,    'g',  NONE,  1, MULT     ) /* local-function-key-control */
_BTE_SEQ(SM_ECMA,                CSI,    'h',  NONE,  0, NONE     ) /* set-mode-ecma */
_BTE_NOQ(SLS,                    CSI,    'h' , NONE,  1, SPACE    ) /* set line spacing */
_BTE_SEQ(SM_DEC,                 CSI,    'h',  WHAT,  0, NONE     ) /* set-mode-dec */
_BTE_NOQ(MC_ECMA,                CSI,    'i',  NONE,  0, NONE     ) /* media-copy-ecma */
_BTE_NOQ(SPH,                    CSI,    'i',  NONE,  1, SPACE    ) /* set page home */
_BTE_NOQ(MC_DEC,                 CSI,    'i',  WHAT,  0, NONE     ) /* media-copy-dec */
_BTE_NOQ(HPB,                    CSI,    'j',  NONE,  0, NONE     ) /* horizontal position backward */
_BTE_NOQ(SPL,                    CSI,    'j',  NONE,  1, SPACE    ) /* set page limit */
_BTE_NOQ(VPB,                    CSI,    'k',  NONE,  0, NONE     ) /* line position backward */
_BTE_SEQ(SCP,                    CSI,    'k',  NONE,  1, SPACE    ) /* select character path */
_BTE_SEQ(RM_ECMA,                CSI,    'l',  NONE,  0, NONE     ) /* reset-mode-ecma */
_BTE_SEQ(RM_DEC,                 CSI,    'l',  WHAT,  0, NONE     ) /* reset-mode-dec */
_BTE_SEQ(SGR,                    CSI,    'm',  NONE,  0, NONE     ) /* select-graphics-rendition */
_BTE_NOQ(DECSGR,                 CSI,    'm',  WHAT,  0, NONE     ) /* DEC select graphics rendition */
_BTE_NOQ(XTERM_SRV,              CSI,    'm',  GT,    0, NONE     ) /* xterm-set-resource-value */
_BTE_SEQ(DSR_ECMA,               CSI,    'n',  NONE,  0, NONE     ) /* device-status-report-ecma */
_BTE_NOQ(XTERM_RRV,              CSI,    'n',  GT,    0, NONE     ) /* xterm-reset-resource-value */
_BTE_SEQ(DSR_DEC,                CSI,    'n',  WHAT,  0, NONE     ) /* device-status-report-dec */
_BTE_NOQ(DAQ,                    CSI,    'o',  NONE,  0, NONE     ) /* define area qualification */
_BTE_NOQ(DECSSL,                 CSI,    'p',  NONE,  0, NONE     ) /* select-setup-language */
_BTE_NOQ(DECSSCLS,               CSI,    'p',  NONE,  1, SPACE    ) /* set-scroll-speed */
_BTE_SEQ(DECSTR,                 CSI,    'p',  NONE,  1, BANG     ) /* soft-terminal-reset */
_BTE_SEQ(DECSCL,                 CSI,    'p',  NONE,  1, DQUOTE   ) /* select-conformance-level */
_BTE_SEQ(DECRQM_ECMA,            CSI,    'p',  NONE,  1, CASH     ) /* request-mode-ecma */
_BTE_NOQ(DECSDPT,                CSI,    'p',  NONE,  1, PCLOSE   ) /* select-digital-printed-data-type */
_BTE_NOQ(DECSPPCS,               CSI,    'p',  NONE,  1, MULT     ) /* select-pro-printer-character-set */
_BTE_SEQ(DECSR,                  CSI,    'p',  NONE,  1, PLUS     ) /* secure-reset */
_BTE_NOQ(DECLTOD,                CSI,    'p',  NONE,  1, COMMA    ) /* load-time-of-day */
_BTE_NOQ(DECARR,                 CSI,    'p',  NONE,  1, MINUS    ) /* auto repeat rate */
_BTE_NOQ(XTERM_PTRMODE,          CSI,    'p',  GT,    0, NONE     ) /* xterm set pointer mode */
_BTE_SEQ(DECRQM_DEC,             CSI,    'p',  WHAT,  1, CASH     ) /* request-mode-dec */
_BTE_NOQ(DECLL,                  CSI,    'q',  NONE,  0, NONE     ) /* load-leds */
_BTE_SEQ(DECSCUSR,               CSI,    'q',  NONE,  1, SPACE    ) /* set-cursor-style */
_BTE_NOQ(DECSCA,                 CSI,    'q',  NONE,  1, DQUOTE   ) /* select-character-protection-attribute */
_BTE_NOQ(DECSDDT,                CSI,    'q',  NONE,  1, CASH     ) /* select-disconnect-delay-time */
_BTE_SEQ(DECSR,                  CSI,    'q',  NONE,  1, MULT     ) /* secure-reset */
_BTE_NOQ(DECELF,                 CSI,    'q',  NONE,  1, PLUS     ) /* enable-local-functions */
_BTE_NOQ(DECTID,                 CSI,    'q',  NONE,  1, COMMA    ) /* select-terminal-id */
_BTE_NOQ(DECCRTST,               CSI,    'q',  NONE,  1, MINUS    ) /* CRT saver time */
_BTE_SEQ(DECSTBM,                CSI,    'r',  NONE,  0, NONE     ) /* set-top-and-bottom-margins */
_BTE_NOQ(DECSKCV,                CSI,    'r',  NONE,  1, SPACE    ) /* set-key-click-volume */
_BTE_NOQ(DECCARA,                CSI,    'r',  NONE,  1, CASH     ) /* change-attributes-in-rectangular-area */
_BTE_NOQ(DECSCS,                 CSI,    'r',  NONE,  1, MULT     ) /* select-communication-speed */
_BTE_NOQ(DECSMKR,                CSI,    'r',  NONE,  1, PLUS     ) /* select-modifier-key-reporting */
_BTE_NOQ(DECSEST,                CSI,    'r',  NONE,  1, MINUS    ) /* energy saver time */
_BTE_SEQ(DECPCTERM_OR_XTERM_RPM, CSI,    'r',  WHAT,  0, NONE     ) /* pcterm or xterm restore DEC private mode */
_BTE_SEQ(DECSLRM_OR_SCOSC,       CSI,    's',  NONE,  0, NONE     ) /* set left and right margins or SCO save cursor */
_BTE_NOQ(DECSPRTT,               CSI,    's',  NONE,  1, CASH     ) /* select-printer-type */
_BTE_NOQ(DECSFC,                 CSI,    's',  NONE,  1, MULT     ) /* select-flow-control */
_BTE_SEQ(XTERM_SPM,              CSI,    's',  WHAT,  0, NONE     ) /* xterm save private mode */
_BTE_SEQ(XTERM_WM,               CSI,    't',  NONE,  0, NONE     ) /* xterm-window-management */
_BTE_NOQ(DECSWBV,                CSI,    't',  NONE,  1, SPACE    ) /* set-warning-bell-volume */
_BTE_NOQ(DECSRFR,                CSI,    't',  NONE,  1, DQUOTE   ) /* select-refresh-rate */
_BTE_NOQ(DECRARA,                CSI,    't',  NONE,  1, CASH     ) /* reverse-attributes-in-rectangular-area */
_BTE_NOQ(XTERM_STM,              CSI,    't',  GT,    0, NONE     ) /* xterm-set-title-mode */
_BTE_SEQ(SCORC,                  CSI,    'u',  NONE,  0, NONE     ) /* SCO restore cursor */
_BTE_NOQ(DECSMBV,                CSI,    'u',  NONE,  1, SPACE    ) /* set-margin-bell-volume */
_BTE_NOQ(DECSTRL,                CSI,    'u',  NONE,  1, DQUOTE   ) /* set-transmit-rate-limit */
_BTE_SEQ(DECRQTSR,               CSI,    'u',  NONE,  1, CASH     ) /* request-terminal-state-report */
_BTE_NOQ(DECSCP,                 CSI,    'u',  NONE,  1, MULT     ) /* select-communication-port */
_BTE_NOQ(DECRQKT,                CSI,    'u',  NONE,  1, COMMA    ) /* request-key-type */
_BTE_NOQ(DECRQUPSS,              CSI,    'u',  WHAT,  0, NONE     ) /* request-user-preferred-supplemental-set */
_BTE_NOQ(DECSLCK,                CSI,    'v',  NONE,  1, SPACE    ) /* set-lock-key-style */
_BTE_NOQ(DECRQDE,                CSI,    'v',  NONE,  1, DQUOTE   ) /* request-display-extent */
_BTE_NOQ(DECCRA,                 CSI,    'v',  NONE,  1, CASH     ) /* copy-rectangular-area */
_BTE_NOQ(DECRPKT,                CSI,    'v',  NONE,  1, COMMA    ) /* report-key-type */
_BTE_NOQ(WYCAA,                  CSI,    'w',  NONE,  0, NONE     ) /* redefine character display attribute association */
_BTE_NOQ(DECRPDE,                CSI,    'w',  NONE,  1, DQUOTE   ) /* report displayed extent */
_BTE_NOQ(DECRQPSR,               CSI,    'w',  NONE,  1, CASH     ) /* request-presentation-state-report */
_BTE_NOQ(DECEFR,                 CSI,    'w',  NONE,  1, SQUOTE   ) /* enable-filter-rectangle */
_BTE_NOQ(DECSPP,                 CSI,    'w',  NONE,  1, PLUS     ) /* set-port-parameter */
_BTE_SEQ(DECREQTPARM,            CSI,    'x',  NONE,  0, NONE     ) /* request-terminal-parameters */
_BTE_NOQ(DECFRA,                 CSI,    'x',  NONE,  1, CASH     ) /* fill-rectangular-area */
_BTE_NOQ(DECES,                  CSI,    'x',  NONE,  1, AND      ) /* enable session */
_BTE_NOQ(DECSACE,                CSI,    'x',  NONE,  1, MULT     ) /* select-attribute-change-extent */
_BTE_NOQ(DECRQPKFM,              CSI,    'x',  NONE,  1, PLUS     ) /* request-program-key-free-memory */
_BTE_NOQ(DECSPMA,                CSI,    'x',  NONE,  1, COMMA    ) /* session page memory allocation */
_BTE_NOQ(DECTST,                 CSI,    'y',  NONE,  0, NONE     ) /* invoke-confidence-test */
_BTE_NOQ(XTERM_CHECKSUM_MODE,    CSI,    'y',  NONE,  1, HASH     ) /* xterm DECRQCRA checksum mode */
_BTE_SEQ(DECRQCRA,               CSI,    'y',  NONE,  1, MULT     ) /* request-checksum-of-rectangular-area */
_BTE_NOQ(DECPKFMR,               CSI,    'y',  NONE,  1, PLUS     ) /* program-key-free-memory-report */
_BTE_NOQ(DECUS,                  CSI,    'y',  NONE,  1, COMMA    ) /* update session */
_BTE_NOQ(WYSCRATE,               CSI,    'z',  NONE,  0, NONE     ) /* set smooth scroll rate */
_BTE_NOQ(DECERA,                 CSI,    'z',  NONE,  1, CASH     ) /* erase-rectangular-area */
_BTE_NOQ(DECELR,                 CSI,    'z',  NONE,  1, SQUOTE   ) /* enable-locator-reporting */
_BTE_NOQ(DECINVM,                CSI,    'z',  NONE,  1, MULT     ) /* invoke-macro */
_BTE_NOQ(DECPKA,                 CSI,    'z',  NONE,  1, PLUS     ) /* program-key-action */
_BTE_NOQ(DECDLDA,                CSI,    'z',  NONE,  1, COMMA    ) /* down line load allocation */
_BTE_NOQ(XTERM_SGR_STACK_PUSH,   CSI,    '{',  NONE,  1, HASH     ) /* push SGR stack */
_BTE_NOQ(DECSERA,                CSI,    '{',  NONE,  1, CASH     ) /* selective-erase-rectangular-area */
_BTE_NOQ(DECSLE,                 CSI,    '{',  NONE,  1, SQUOTE   ) /* select-locator-events */
_BTE_NOQ(DECSTGLT,               CSI,    '{',  NONE,  1, PCLOSE   ) /* select color lookup table */
_BTE_NOQ(DECSZS,                 CSI,    '{',  NONE,  1, COMMA    ) /* select zero symbol */
_BTE_NOQ(XTERM_SGR_REPORT,       CSI,    '|',  NONE,  1, HASH     ) /* SGR report */
_BTE_NOQ(DECSCPP,                CSI,    '|',  NONE,  1, CASH     ) /* select-columns-per-page */
_BTE_NOQ(DECRQLP,                CSI,    '|',  NONE,  1, SQUOTE   ) /* request-locator-position */
_BTE_NOQ(DECSNLS,                CSI,    '|',  NONE,  1, MULT     ) /* set-lines-per-screen */
_BTE_NOQ(DECAC,                  CSI,    '|',  NONE,  1, COMMA    ) /* assign color */
_BTE_NOQ(DECKBD,                 CSI,    '}',  NONE,  1, SPACE    ) /* keyboard-language-selection */
_BTE_NOQ(XTERM_SGR_STACK_POP,    CSI,    '}',  NONE,  1, HASH     ) /* pop SGR stack */
_BTE_NOQ(DECSASD,                CSI,    '}',  NONE,  1, CASH     ) /* select-active-status-display */
_BTE_NOQ(DECIC,                  CSI,    '}',  NONE,  1, SQUOTE   ) /* insert-column */
_BTE_NOQ(DECATC,                 CSI,    '}',  NONE,  1, COMMA    ) /* alternate text color */
_BTE_NOQ(DECTME,                 CSI,    '~',  NONE,  1, SPACE    ) /* terminal-mode-emulation */
_BTE_NOQ(DECSSDT,                CSI,    '~',  NONE,  1, CASH     ) /* select-status-display-line-type */
_BTE_NOQ(DECDC,                  CSI,    '~',  NONE,  1, SQUOTE   ) /* delete-column */
_BTE_NOQ(DECPS,                  CSI,    '~',  NONE,  1, COMMA    ) /* play sound */
