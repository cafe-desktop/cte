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

#pragma once

#include <cstdint>
#include <cstdio>

#include "parser-arg.hh"
#include "parser-string.hh"

struct bte_parser_t;
struct bte_seq_t;

/*
 * Parsers
 * The bte_parser object parses control-sequences for both host and terminal
 * side. Based on this parser, there is a set of command-parsers that take a
 * bte_seq sequence and returns the command it represents. This is different
 * for host and terminal side, and so far we only provide the terminal side, as
 * host side is not used by anyone.
 */

#define BTE_PARSER_ARG_MAX (32)

enum {
        BTE_SEQ_NONE,        /* placeholder, no sequence parsed */

        BTE_SEQ_IGNORE,      /* no-op character */
        BTE_SEQ_GRAPHIC,     /* graphic character */
        BTE_SEQ_CONTROL,     /* control character */
        BTE_SEQ_ESCAPE,      /* escape sequence */
        BTE_SEQ_CSI,         /* control sequence function */
        BTE_SEQ_DCS,         /* device control string */
        BTE_SEQ_OSC,         /* operating system control */
        BTE_SEQ_SCI,         /* single character control function */
        BTE_SEQ_APC,         /* application program command */
        BTE_SEQ_PM,          /* privacy message */
        BTE_SEQ_SOS,         /* start of string */

        BTE_SEQ_N,
};

enum {
        BTE_SEQ_INTERMEDIATE_CHAR_NONE    = 0,

        BTE_SEQ_INTERMEDIATE_CHAR_SPACE   = ' ',  /* 02/00 */
        BTE_SEQ_INTERMEDIATE_CHAR_BANG    = '!',  /* 02/01 */
        BTE_SEQ_INTERMEDIATE_CHAR_DQUOTE  = '"',  /* 02/02 */
        BTE_SEQ_INTERMEDIATE_CHAR_HASH    = '#',  /* 02/03 */
        BTE_SEQ_INTERMEDIATE_CHAR_CASH    = '$',  /* 02/04 */
        BTE_SEQ_INTERMEDIATE_CHAR_PERCENT = '%',  /* 02/05 */
        BTE_SEQ_INTERMEDIATE_CHAR_AND     = '&',  /* 02/06 */
        BTE_SEQ_INTERMEDIATE_CHAR_SQUOTE  = '\'', /* 02/07 */
        BTE_SEQ_INTERMEDIATE_CHAR_POPEN   = '(',  /* 02/08 */
        BTE_SEQ_INTERMEDIATE_CHAR_PCLOSE  = ')',  /* 02/09 */
        BTE_SEQ_INTERMEDIATE_CHAR_MULT    = '*',  /* 02/10 */
        BTE_SEQ_INTERMEDIATE_CHAR_PLUS    = '+',  /* 02/11 */
        BTE_SEQ_INTERMEDIATE_CHAR_COMMA   = ',',  /* 02/12 */
        BTE_SEQ_INTERMEDIATE_CHAR_MINUS   = '-',  /* 02/13 */
        BTE_SEQ_INTERMEDIATE_CHAR_DOT     = '.',  /* 02/14 */
        BTE_SEQ_INTERMEDIATE_CHAR_SLASH   = '/',  /* 02/15 */
};

enum {
        BTE_SEQ_PARAMETER_CHAR_NONE  = 0,

        /* Numbers; not used         *  03/00..03/09 */
        /* COLON is reserved         = ':'   * 03/10 */
        /* SEMICOLON is reserved     = ';'   * 03/11 */
        BTE_SEQ_PARAMETER_CHAR_LT    = '<', /* 03/12 */
        BTE_SEQ_PARAMETER_CHAR_EQUAL = '=', /* 03/13 */
        BTE_SEQ_PARAMETER_CHAR_GT    = '>', /* 03/14 */
        BTE_SEQ_PARAMETER_CHAR_WHAT  = '?'  /* 03/15 */
};

#define BTE_SEQ_MAKE_INTERMEDIATE(c) ((c) - ' ' + 1)

enum {
        BTE_SEQ_INTERMEDIATE_NONE      = 0,

        BTE_SEQ_INTERMEDIATE_SPACE     = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_SPACE  ),
        BTE_SEQ_INTERMEDIATE_BANG      = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_BANG   ),
        BTE_SEQ_INTERMEDIATE_DQUOTE    = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_DQUOTE ),
        BTE_SEQ_INTERMEDIATE_HASH      = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_HASH   ),
        BTE_SEQ_INTERMEDIATE_CASH      = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_CASH   ),
        BTE_SEQ_INTERMEDIATE_PERCENT   = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_PERCENT),
        BTE_SEQ_INTERMEDIATE_AND       = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_AND    ),
        BTE_SEQ_INTERMEDIATE_SQUOTE    = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_SQUOTE ),
        BTE_SEQ_INTERMEDIATE_POPEN     = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_POPEN  ),
        BTE_SEQ_INTERMEDIATE_PCLOSE    = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_PCLOSE ),
        BTE_SEQ_INTERMEDIATE_MULT      = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_MULT   ),
        BTE_SEQ_INTERMEDIATE_PLUS      = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_PLUS   ),
        BTE_SEQ_INTERMEDIATE_COMMA     = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_COMMA  ),
        BTE_SEQ_INTERMEDIATE_MINUS     = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_MINUS  ),
        BTE_SEQ_INTERMEDIATE_DOT       = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_DOT    ),
        BTE_SEQ_INTERMEDIATE_SLASH     = BTE_SEQ_MAKE_INTERMEDIATE(BTE_SEQ_INTERMEDIATE_CHAR_SLASH  ),
};

#define BTE_SEQ_MAKE_PARAMETER(c) ('?' - (c) + 1)

enum {
        BTE_SEQ_PARAMETER_NONE  = 0,

        BTE_SEQ_PARAMETER_LT    = BTE_SEQ_MAKE_PARAMETER(BTE_SEQ_PARAMETER_CHAR_LT   ),
        BTE_SEQ_PARAMETER_EQUAL = BTE_SEQ_MAKE_PARAMETER(BTE_SEQ_PARAMETER_CHAR_EQUAL),
        BTE_SEQ_PARAMETER_GT    = BTE_SEQ_MAKE_PARAMETER(BTE_SEQ_PARAMETER_CHAR_GT   ),
        BTE_SEQ_PARAMETER_WHAT  = BTE_SEQ_MAKE_PARAMETER(BTE_SEQ_PARAMETER_CHAR_WHAT ),
};

enum {
#define _BTE_CMD(cmd) BTE_CMD_##cmd,
#define _BTE_NOP(cmd) BTE_CMD_##cmd,
#include "parser-cmd.hh"
#undef _BTE_CMD
#undef _BTE_NOP

        BTE_CMD_N,
        BTE_CMD_NOP_FIRST = BTE_CMD_ACK
};

enum {
#define _BTE_REPLY(cmd,type,final,pintro,intermediate,code) BTE_REPLY_##cmd,
#include "parser-reply.hh"
#undef _BTE_REPLY

        BTE_REPLY_N
};

enum {
#define _BTE_CHARSET_PASTE(name) BTE_CHARSET_##name,
#define _BTE_CHARSET(name) _BTE_CHARSET_PASTE(name)
#define _BTE_CHARSET_ALIAS_PASTE(name1,name2) BTE_CHARSET_##name1 = BTE_CHARSET_##name2,
#define _BTE_CHARSET_ALIAS(name1,name2) _BTE_CHARSET_ALIAS_PASTE(name1,name2)
#include "parser-charset.hh"
#undef _BTE_CHARSET_PASTE
#undef _BTE_CHARSET
#undef _BTE_CHARSET_ALIAS_PASTE
#undef _BTE_CHARSET_ALIAS
};

enum {
#define _BTE_OSC(osc,value) BTE_OSC_##osc = value,
#include "parser-osc.hh"
#undef _BTE_OSC

        BTE_OSC_N
};

enum {
#define _BTE_SGR(name, value) BTE_SGR_##name = value,
#define _BTE_NGR(...)
#include "parser-sgr.hh"
#undef _BTE_SGR
#undef _BTE_NGR
};

enum {
#define _BTE_SGR(name, value) BTE_DECSGR_##name = value,
#define _BTE_NGR(...)
#include "parser-decsgr.hh"
#undef _BTE_SGR
#undef _BTE_NGR
};

#define BTE_CHARSET_CHARSET_MASK   ((1U << 16) - 1U)
#define BTE_CHARSET_SLOT_OFFSET    (16)
#define BTE_CHARSET_GET_CHARSET(c) ((c) & BTE_CHARSET_CHARSET_MASK)
#define BTE_CHARSET_GET_SLOT(c)    ((c) >> BTE_CHARSET_SLOT_OFFSET)

struct bte_seq_t {
        unsigned int type;
        unsigned int command;
        uint32_t terminator;
        unsigned int intermediates;
        unsigned int n_intermediates;
        unsigned int charset;
        unsigned int n_args;
        unsigned int n_final_args;
        bte_seq_arg_t args[BTE_PARSER_ARG_MAX];
        bte_seq_string_t arg_str;
        uint32_t introducer;
};

struct bte_parser_t {
        bte_seq_t seq;
        unsigned int state;
};

void bte_parser_init(bte_parser_t* parser);
void bte_parser_deinit(bte_parser_t* parser);
int bte_parser_feed(bte_parser_t* parser,
                    uint32_t raw);
void bte_parser_reset(bte_parser_t* parser);
