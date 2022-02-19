//
// Created by carlos on 2/18/22.
//

#ifndef TIPO_LEX_H
#define TIPO_LEX_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

enum token_type {
    TT_SOF,
    TT_EOF,
    TT_EOL,
    TT_ID,
    TT_LPAR,
    TT_RPAR,
    TT_DEF,
    TT_BODY,
};

struct token {
    const char *buf;
    size_t len;
    size_t row;
    size_t col;
    enum token_type typ;
};

struct token next_token(struct token prev);
struct token first_token(const char *buf);

#define lex_error(msg, ...) do{ printf("%lu:%lu: " msg "\n", cur_row, cur_col, ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_LEX_H
