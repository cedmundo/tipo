//
// Created by carlos on 2/18/22.
//

#include "lex.h"
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
    enum token_type cur_typ = TT_EOF;
    size_t cur_col = prev.col + prev.len;
    size_t cur_row = prev.row;
    size_t cur_len = 0L;

    // remove spaces from token
    while (is_space(*cur_buf)) {
        base_buf ++;
        cur_buf ++;
        cur_col ++;
    }

    // collect and group the end of lines
    if (is_eol(*cur_buf)) {
        cur_typ = TT_EOL;
        cur_len = 1L;
        if (*cur_buf == '\n') {
            cur_row ++;
            cur_col = 1L;
        }

        while (is_eol(*(cur_buf + 1))) {
            cur_buf ++;
            cur_len ++;
            if (*cur_buf == '\n') {
                cur_row ++;
                cur_col = 1L;
            }
        }
    } else if (is_alpha(*cur_buf)) {
        cur_typ = TT_ID;
        cur_len = 1L;
        while (is_alphanum(*(cur_buf + 1))) {
            cur_buf ++;
            cur_len ++;
        }
    } else if (is_punct(*cur_buf)) {
        cur_len = 1L;
        if (*cur_buf == '\\') {
            cur_typ = TT_DEF;
        } else if (*cur_buf == '(') {
            cur_typ = TT_LPAR;
        } else if (*cur_buf == ')') {
            cur_typ = TT_RPAR;
        } else if (*cur_buf == '.' || *cur_buf == '=') {
            cur_typ = TT_BODY;
        } else {
            assert(0 && "Undefined punctuation symbol");
        }
    } else if (!is_eof(*cur_buf)) {
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
