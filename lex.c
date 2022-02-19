//
// Created by carlos on 2/18/22.
//

#include "lex.h"
#include "utf8.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static bool is_alpha(uint32_t cp) {
    return (cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z') || cp == '_' || cp == '$' || cp == '@';
}

static bool is_digit(uint32_t cp) {
    return cp >= '0' && cp <= '9';
}

static bool is_space(uint32_t cp) {
    return cp == ' ' || cp == '\t';
}

static bool is_eol(uint32_t cp) {
    return cp == ';' || cp == '\r' || cp == '\n';
}

static bool is_eof(uint32_t cp) {
    return cp == 0;
}

static bool is_punct(uint32_t cp) {
    return cp == '(' || cp == ')' || cp == '\\' || cp == '.' || cp == '=' || cp == 0x03BB;
}

static bool is_comment_start(uint32_t cp) {
    return cp == '#';
}

static bool is_alphanum(uint32_t cp) {
    return is_alpha(cp) || is_digit(cp);
}

struct token first_token(const char *buf) {
    return (struct token) {
        .typ = TT_SOF,
        .buf = buf,
        .len = 0L,
        .col = 1L,
        .row = 1L,
    };
}

struct token next_token(struct token prev) {
    const char *base_buf = prev.buf + prev.len;
    const char *cur_buf = base_buf;
    uint32_t cur_cp = 0L;
    uint8_t cur_inc;

    enum token_type cur_typ = TT_EOF;
    size_t cur_col = prev.col + prev.len;
    size_t cur_row = prev.row;
    size_t cur_len = 0L;

    cur_inc = dec_bytes_to_cp(cur_buf, &cur_cp);

    // remove spaces from token
    for (;;) {
        if (!is_space(cur_cp)) {
            break;
        }

        base_buf += cur_inc;
        cur_buf += cur_inc;
        cur_inc = dec_bytes_to_cp(cur_buf, &cur_cp);
        cur_col ++;
    }

    // ignore comments
    if (is_comment_start(cur_cp)) {
        base_buf += cur_inc;
        cur_buf += cur_inc;

        for(;;) {
            if (cur_cp == '\n') {
                base_buf += cur_inc;
                cur_buf += cur_inc;
                cur_inc = dec_bytes_to_cp(cur_buf, &cur_cp);
                cur_row ++;
                cur_col = 1L;
                break;
            }

            base_buf += cur_inc;
            cur_buf += cur_inc;
            cur_inc = dec_bytes_to_cp(cur_buf, &cur_cp);
            cur_col ++;
        }
    }

    // collect and group the end of lines
    if (is_eol(cur_cp)) {
        cur_typ = TT_EOL;
        cur_len += cur_inc;
        if (cur_cp == '\n') {
            cur_row ++;
            cur_col = 1L;
        }

        for (;;) {
            uint32_t ahead_cp;
            uint8_t ahead_inc = dec_bytes_to_cp(cur_buf + cur_inc, &ahead_cp);
            if (!is_eol(ahead_cp)) {
                break;
            }

            cur_buf += cur_inc;
            cur_len += cur_inc;
            cur_inc = ahead_inc;

            if (cur_cp == '\n') {
                cur_row ++;
                cur_col = 1L;
            }
        }
    } else if (is_alpha(cur_cp)) {
        cur_typ = TT_ID;
        cur_len += cur_inc;
        for (;;) {
            uint32_t ahead_cp;
            uint8_t ahead_inc = dec_bytes_to_cp(cur_buf + cur_inc, &ahead_cp);
            if (!is_alphanum(ahead_cp)) {
                break;
            }

            cur_buf += cur_inc;
            cur_len += cur_inc;
            cur_inc = ahead_inc;
        }
    } else if (is_punct(cur_cp)) {
        cur_len += cur_inc;
        if (cur_cp == '\\' || cur_cp == 0x03BB) {
            cur_typ = TT_DEF;
        } else if (cur_cp == '(') {
            cur_typ = TT_LPAR;
        } else if (cur_cp == ')') {
            cur_typ = TT_RPAR;
        } else if (cur_cp == '.' || cur_cp == '=') {
            cur_typ = TT_BODY;
        } else {
            assert(0 && "Undefined punctuation symbol");
        }
    } else if (!is_eof(cur_cp)) {
        lex_error("error: unexpected token (ascii=%d): '%c'", *cur_buf, *cur_buf);
    }

    return (struct token) {
            .typ = cur_typ,
            .len = cur_len,
            .buf = base_buf,
            .col = cur_col,
            .row = cur_row,
    };
}
