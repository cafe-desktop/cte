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

#if !defined(_BTE_CMD) || !defined(_BTE_NOP)
#error "Must define _BTE_CMD and _BTE_NOP before including this file"
#endif

/* Implemented in BTE: */

_BTE_CMD(NONE) /* placeholder */
_BTE_CMD(GRAPHIC) /* graphics character */
_BTE_CMD(ACS) /* announce code structure */
_BTE_CMD(BEL) /* bell */
_BTE_CMD(BS) /* backspace */
_BTE_CMD(CBT) /* cursor backward tabulation */
_BTE_CMD(CHA) /* cursor horizontal absolute */
_BTE_CMD(CHT) /* cursor horizontal forward tabulation */
_BTE_CMD(CNL) /* cursor next line */
_BTE_CMD(CPL) /* cursor previous line */
_BTE_CMD(CR) /* carriage return */
_BTE_CMD(CTC) /* cursor tabulation control */
_BTE_CMD(CUB) /* cursor backward */
_BTE_CMD(CUD) /* cursor down */
_BTE_CMD(CUF) /* cursor forward */
_BTE_CMD(CUP) /* cursor position */
_BTE_CMD(CUU) /* cursor up */
_BTE_CMD(CnD) /* Cn-designate */
_BTE_CMD(DA1) /* primary device attributes */
_BTE_CMD(DA2) /* secondary device attributes */
_BTE_CMD(DA3) /* tertiary device attributes */
_BTE_CMD(DCH) /* delete character */
_BTE_CMD(DECALN) /* screen alignment pattern */
_BTE_CMD(DECBI) /* back index */
_BTE_CMD(DECKPAM) /* keypad application mode */
_BTE_CMD(DECKPNM) /* keypad numeric mode */
_BTE_CMD(DECPCTERM_OR_XTERM_RPM) /* pcterm or xterm restore DEC private mode */
_BTE_CMD(DECRC) /* restore cursor */
_BTE_CMD(DECREQTPARM) /* request terminal parameters */
_BTE_CMD(DECRQCRA) /* request checksum of rectangular area */
_BTE_CMD(DECRQM_DEC) /* request mode dec */
_BTE_CMD(DECRQM_ECMA) /* request mode ecma */
_BTE_CMD(DECRQSS) /* request selection or setting */
_BTE_CMD(DECRQTSR) /* request terminal state report */
_BTE_CMD(DECSCL) /* select conformance level */
_BTE_CMD(DECSC) /* save cursor */
_BTE_CMD(DECSCUSR) /* set cursor style */
_BTE_CMD(DECSED) /* selective erase in display */
_BTE_CMD(DECSEL) /* selective erase in line */
_BTE_CMD(DECSGR) /* DEC select graphics rendition */
_BTE_CMD(DECSLPP) /* set lines per page */
_BTE_CMD(DECSLRM_OR_SCOSC) /* set left and right margins or SCO save cursor */
_BTE_CMD(DECSR) /* secure reset */
_BTE_CMD(DECST8C) /* set tab at every 8 columns */
_BTE_CMD(DECSTBM) /* set top and bottom margins */
_BTE_CMD(DECSTR) /* soft terminal reset */
_BTE_CMD(DL) /* delete line */
_BTE_CMD(DOCS) /* designate-other-coding-system */
_BTE_CMD(DSR_DEC) /* device status report dec */
_BTE_CMD(DSR_ECMA) /* device status report ecma */
_BTE_CMD(ECH) /* erase character */
_BTE_CMD(ED) /* erase in display */
_BTE_CMD(EL) /* erase in line */
_BTE_CMD(FF) /* form feed */
_BTE_CMD(GnDm) /* Gn-designate-9m-charset */
_BTE_CMD(GnDMm) /* Gn-designate-multibyte-9m-charset */
_BTE_CMD(HPA) /* horizontal position absolute */
_BTE_CMD(HPR) /* horizontal position relative */
_BTE_CMD(HT) /* horizontal tab */
_BTE_CMD(HTJ) /* character tabulation with justification */
_BTE_CMD(HTS) /* horizontal tab set */
_BTE_CMD(HVP) /* horizontal and vertical position */
_BTE_CMD(ICH) /* insert character */
_BTE_CMD(IL) /* insert line */
_BTE_CMD(IND) /* index */
_BTE_CMD(LF) /* line feed */
_BTE_CMD(LS0) /* locking shift 0 */
_BTE_CMD(LS1) /* locking shift 1 */
_BTE_CMD(LS1R) /* locking shift 1 right */
_BTE_CMD(LS2) /* locking shift 2 */
_BTE_CMD(LS2R) /* locking shift 2 right */
_BTE_CMD(LS3) /* locking shift 3 */
_BTE_CMD(LS3R) /* locking shift 3 right */
_BTE_CMD(NEL) /* next line */
_BTE_CMD(OSC) /* operating-system-command */
_BTE_CMD(REP) /* repeat */
_BTE_CMD(RI) /* reverse index */
_BTE_CMD(RIS) /* reset to initial state */
_BTE_CMD(RM_DEC) /* reset mode dec */
_BTE_CMD(RM_ECMA) /* reset mode ecma */
_BTE_CMD(SCORC) /* SCO restore cursor */
_BTE_CMD(SCOSC) /* SCO save cursor */
_BTE_CMD(SCP) /* select character path */
_BTE_CMD(SD) /* scroll down */
_BTE_CMD(SD_OR_XTERM_IHMT) /* scroll down or xterm initiate highlight mouse tracking */
_BTE_CMD(SGR) /* select graphics rendition */
_BTE_CMD(SM_DEC) /* set mode dec */
_BTE_CMD(SM_ECMA) /* set mode ecma */
_BTE_CMD(SPD) /* select presentation directions */
_BTE_CMD(SS2) /* single shift 2 */
_BTE_CMD(SS3) /* single shift 3 */
_BTE_CMD(SUB) /* substitute */
_BTE_CMD(SU) /* scroll up */
_BTE_CMD(TBC) /* tab clear */
_BTE_CMD(TCC) /* tabulation centred on character */
_BTE_CMD(TSR) /* tabulation stop remove */
_BTE_CMD(VPA) /* vertical line position absolute */
_BTE_CMD(VPR) /* vertical line position relative */
_BTE_CMD(VT) /* vertical tab */
_BTE_CMD(XTERM_RPM) /* xterm restore DEC private mode */
_BTE_CMD(XTERM_SPM) /* xterm save DEC private mode */
_BTE_CMD(XTERM_WM) /* xterm window management */

/* Unimplemented in BTE: */

_BTE_NOP(ACK) /* acknowledge */
_BTE_NOP(BPH) /* break permitted here */
_BTE_NOP(CCH) /* cancel character */
_BTE_NOP(CMD) /* coding method delimiter */
_BTE_NOP(CVT) /* cursor line tabulation */
_BTE_NOP(DAQ) /* define area qualification */
_BTE_NOP(DC1) /* device control 1 / XON */
_BTE_NOP(DC2) /* devince control 2 */
_BTE_NOP(DC3) /* device control 3 / XOFF */
_BTE_NOP(DC4) /* device control 4 */
_BTE_NOP(DECAC) /* assign color */
_BTE_NOP(DECANM) /* ansi mode */
_BTE_NOP(DECARR) /* auto repeat rate */
_BTE_NOP(DECATC) /* alternate text color */
_BTE_NOP(DECAUPSS) /* assign user preferred supplemental sets */
_BTE_NOP(DECCARA) /* change attributes in rectangular area */
_BTE_NOP(DECCKD) /* copy key default */
_BTE_NOP(DECCRA) /* copy rectangular area */
_BTE_NOP(DECCRTST) /* CRT saver time */
_BTE_NOP(DECDC) /* delete column */
_BTE_NOP(DECDHL_BH) /* double width double height line: bottom half */
_BTE_NOP(DECDHL_TH) /* double width double height line: top half */
_BTE_NOP(DECDLDA) /* down line load allocation */
_BTE_NOP(DECDLD) /* dynamically redefinable character sets extension */
_BTE_NOP(DECDMAC) /* define macro */
_BTE_NOP(DECDWL) /* double width single height line */
_BTE_NOP(DECEFR) /* enable filter rectangle */
_BTE_NOP(DECELF) /* enable local functions */
_BTE_NOP(DECELR) /* enable locator reporting */
_BTE_NOP(DECERA) /* erase rectangular area */
_BTE_NOP(DECES) /* enable session */
_BTE_NOP(DECFI) /* forward index */
_BTE_NOP(DECFNK) /* function key */
_BTE_NOP(DECFRA) /* fill rectangular area */
_BTE_NOP(DECIC) /* insert column */
_BTE_NOP(DECINVM) /* invoke macro */
_BTE_NOP(DECKBD) /* keyboard language selection */
_BTE_NOP(DECLANS) /* load answerback message */
_BTE_NOP(DECLBAN) /* load banner message */
_BTE_NOP(DECLBD) /* locator button define */
_BTE_NOP(DECLFKC) /* local function key control */
_BTE_NOP(DECLL) /* load leds */
_BTE_NOP(DECLTOD) /* load time of day */
_BTE_NOP(DECPAK) /* program alphanumeric key */
_BTE_NOP(DECPCTERM) /* pcterm */
_BTE_NOP(DECPFK) /* program function key */
_BTE_NOP(DECPKA) /* program key action */
_BTE_NOP(DECPKFMR) /* program key free memory report */
_BTE_NOP(DECPS) /* play sound */
_BTE_NOP(DECRARA) /* reverse attributes in rectangular area */
_BTE_NOP(DECREGIS) /* ReGIS graphics */
_BTE_NOP(DECRPAK) /* report all modifier/alphanumeric key state */
_BTE_NOP(DECRPDE) /* report displayed extent */
_BTE_NOP(DECRPFK) /* report function key definition */
_BTE_NOP(DECRPKT) /* report key type */
_BTE_NOP(DECRQDE) /* request display extent */
_BTE_NOP(DECRQKT) /* request key type */
_BTE_NOP(DECRQLP) /* request locator position */
_BTE_NOP(DECRQPKFM) /* request program key free memory */
_BTE_NOP(DECRQPSR) /* request presentation state report */
_BTE_NOP(DECRQUPSS) /* request user preferred supplemental set */
_BTE_NOP(DECRSPS) /* restore presentation state */
_BTE_NOP(DECRSTS) /* restore terminal state */
_BTE_NOP(DECSACE) /* select attribute change extent */
_BTE_NOP(DECSASD) /* select active status display */
_BTE_NOP(DECSCA) /* select character protection attribute */
_BTE_NOP(DECSCPP) /* select columns per page */
_BTE_NOP(DECSCP) /* select communication port */
_BTE_NOP(DECSCS) /* select communication speed */
_BTE_NOP(DECSDDT) /* select disconnect delay time */
_BTE_NOP(DECSDPT) /* select digital printed data type */
_BTE_NOP(DECSERA) /* selective erase rectangular area */
_BTE_NOP(DECSEST) /* energy saver time */
_BTE_NOP(DECSFC) /* select flow control */
_BTE_NOP(DECSIXEL) /* SIXEL graphics */
_BTE_NOP(DECSKCV) /* set key click volume */
_BTE_NOP(DECSLCK) /* set lock key style */
_BTE_NOP(DECSLE) /* select locator events */
_BTE_NOP(DECSLRM) /* set left and right margins */
_BTE_NOP(DECSMBV) /* set margin bell volume */
_BTE_NOP(DECSMKR) /* select modifier key reporting */
_BTE_NOP(DECSNLS) /* set lines per screen */
_BTE_NOP(DECSPMA) /* session page memory allocation */
_BTE_NOP(DECSPPCS) /* select pro printer character set */
_BTE_NOP(DECSPP) /* set port parameter */
_BTE_NOP(DECSPRTT) /* select printer type */
_BTE_NOP(DECSRFR) /* select refresh rate */
_BTE_NOP(DECSSCLS) /* set scroll speed */
_BTE_NOP(DECSSDT) /* select status display line type */
_BTE_NOP(DECSSL) /* select setup language */
_BTE_NOP(DECSTGLT) /* select color lookup table */
_BTE_NOP(DECSTRL) /* set transmit rate limit */
_BTE_NOP(DECSTUI) /* set terminal unit id */
_BTE_NOP(DECSWBV) /* set warning bell volume */
_BTE_NOP(DECSWL) /* single width single height line */
_BTE_NOP(DECSZS) /* select zero symbol */
_BTE_NOP(DECTID) /* select terminal id */
_BTE_NOP(DECTME) /* terminal mode emulation */
_BTE_NOP(DECTST) /* invoke confidence test */
_BTE_NOP(DECUDK) /* user defined keys */
_BTE_NOP(DECUS) /* update session */
_BTE_NOP(DLE) /* data link escape */
_BTE_NOP(DMI) /* disable manual input */
_BTE_NOP(DTA) /* dimension text area */
_BTE_NOP(EA) /* erase in area */
_BTE_NOP(EF) /* erase in field */
_BTE_NOP(EM) /* end of medium */
_BTE_NOP(EMI) /* enable manual input */
_BTE_NOP(ENQ) /* enquire */
_BTE_NOP(EOT) /* end of transmission */
_BTE_NOP(EPA) /* end of guarded area */
_BTE_NOP(ESA) /* end of selected area */
_BTE_NOP(ETB) /* end of transmission block */
_BTE_NOP(ETX) /* end of text */
_BTE_NOP(FNK) /* function key */
_BTE_NOP(FNT) /* font selection */
_BTE_NOP(GCC) /* graphic character combination */
_BTE_NOP(GSM) /* graphic size modification */
_BTE_NOP(GSS) /* graphic size selection */
_BTE_NOP(HPB) /* horizontal position backward */
_BTE_NOP(IDCS) /* identify DCS */
_BTE_NOP(IGS) /* identify graphic subrepertoire */
_BTE_NOP(INT) /* interrupt */
_BTE_NOP(IRR) /* identify-revised-registration */
_BTE_NOP(IS1) /* information separator 1 / unit separator (US) */
_BTE_NOP(IS2) /* information separator 2 / record separator (RS) */
_BTE_NOP(IS3) /* information separator 3 / group separator (GS)*/
_BTE_NOP(IS4) /* information separator 4 / file separator (FS) */
_BTE_NOP(JFY) /* justify */
_BTE_NOP(MC_DEC) /* media copy dec */
_BTE_NOP(MC_ECMA) /* media copy ecma */
_BTE_NOP(MW) /* message waiting */
_BTE_NOP(NAK) /* negative acknowledge */
_BTE_NOP(NBH) /* no break permitted here */
_BTE_NOP(NP) /* next page */
_BTE_NOP(NUL) /* nul */
_BTE_NOP(PEC) /* presentation expand or contract */
_BTE_NOP(PFS) /* page format selection */
_BTE_NOP(PLD) /* partial line forward */
_BTE_NOP(PLU) /* partial line backward */
_BTE_NOP(PPA) /* page position absolute */
_BTE_NOP(PPB) /* page position backward */
_BTE_NOP(PP) /* preceding page */
_BTE_NOP(PPR) /* page position relative */
_BTE_NOP(PTX) /* parallel texts */
_BTE_NOP(PU1) /* private use 1 */
_BTE_NOP(PU2) /* private use 2 */
_BTE_NOP(QUAD) /* quad */
_BTE_NOP(RLOGIN_MML) /* RLogin music macro language */
_BTE_NOP(SACS) /* set additional character separation */
_BTE_NOP(SAPV) /* select alternative presentation variants */
_BTE_NOP(SCO) /* select character orientation */
_BTE_NOP(SCS) /* set character spacing */
_BTE_NOP(SDS) /* start directed string */
_BTE_NOP(SEE) /* select editing extent */
_BTE_NOP(SEF) /* sheet eject and feed */
_BTE_NOP(SHS) /* select character spacing */
_BTE_NOP(SIMD) /* select implicit movement direction */
_BTE_NOP(SLH) /* set line home */
_BTE_NOP(SLL) /* set line limit */
_BTE_NOP(SL) /* scroll left */
_BTE_NOP(SLS) /* set line spacing */
_BTE_NOP(SOH) /* start of heading */
_BTE_NOP(SPA) /* start of protected area */
_BTE_NOP(SPH) /* set page home */
_BTE_NOP(SPI) /* spacing increment */
_BTE_NOP(SPL) /* set page limit */
_BTE_NOP(SPQR) /* select print quality and rapidity */
_BTE_NOP(SRCS) /* set reduced character separation */
_BTE_NOP(SR) /* scroll right */
_BTE_NOP(SRS) /* start reversed string */
_BTE_NOP(SSA) /* start of selected area */
_BTE_NOP(SSU) /* set size unit */
_BTE_NOP(SSW) /* set space width */
_BTE_NOP(ST) /* string terminator */
_BTE_NOP(STAB) /* selective tabulation */
_BTE_NOP(STS) /* set transmit state */
_BTE_NOP(STX) /* start of text */
_BTE_NOP(SVS) /* select line spacing */
_BTE_NOP(SYN) /* synchronize */
_BTE_NOP(TAC) /* tabulation aligned centre */
_BTE_NOP(TALE) /* tabulation aligned leading edge */
_BTE_NOP(TATE) /* tabulation aligned trailing edge */
_BTE_NOP(TSS) /* thine space specification */
_BTE_NOP(VTS) /* line tabulation set */
_BTE_NOP(VPB) /* line position backward */
_BTE_NOP(WYCAA) /* redefine character display attribute association */
_BTE_NOP(WYDHL_BH) /* single width double height line: bottom half */
_BTE_NOP(WYDHL_TH) /* single width double height line: top half */
_BTE_NOP(WYLSFNT) /* load soft font */
_BTE_NOP(WYSCRATE) /* set smooth scroll rate */
_BTE_NOP(XDGSYNC) /* synchronous update */
_BTE_NOP(XTERM_CHECKSUM_MODE) /* xterm DECRQCRA checksum mode */
_BTE_NOP(XTERM_IHMT) /* xterm initiate highlight mouse tracking */
_BTE_NOP(XTERM_MLHP) /* xterm memory lock hp bugfix */
_BTE_NOP(XTERM_MUHP) /* xterm memory unlock hp bugfix */
_BTE_NOP(XTERM_PTRMODE) /* xterm set pointer mode */
_BTE_NOP(XTERM_RQTCAP) /* xterm request termcap/terminfo */
_BTE_NOP(XTERM_RRV) /* xterm reset resource value */
_BTE_NOP(XTERM_RTM) /* xterm reset title mode */
_BTE_NOP(XTERM_SGFX) /* xterm sixel graphics */
_BTE_NOP(XTERM_SGR_STACK_POP) /* xterm pop SGR stack */
_BTE_NOP(XTERM_SGR_STACK_PUSH) /* xterm push SGR stack */
_BTE_NOP(XTERM_SGR_REPORT) /* xterm SGR report */
_BTE_NOP(XTERM_SRV) /* xterm set resource value */
_BTE_NOP(XTERM_STCAP) /* xterm set termcap/terminfo */
_BTE_NOP(XTERM_STM) /* xterm set title mode */
