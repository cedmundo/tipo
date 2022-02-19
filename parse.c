//
// Created by carlos on 2/18/22.
//
#include "lex.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static_assert(NT_COUNT == 4, "Exhaustive node type name table");
static const char *node_type_name_table[] = {
        [NT_ID] = "identity",
        [NT_APPLICATION] = "application",
        [NT_ABSTRACTION] = "abstraction",
        [NT_THEN] = "then",
};

bool check_type(struct token *cur, enum token_type typ) {
    return cur->typ == typ;
}

struct token consume(struct token *cur) {
    struct token tmp = *cur;
    *cur = next_token(*cur);
    return tmp;
}

struct node *new_node() {
    return calloc(1, sizeof(struct node));
}

struct node *new_id_node(struct token token) {
    struct node *node = new_node();
    node->typ = NT_ID;
    node->token = token;
    return node;
}

struct node *new_binary_node(struct token token, enum node_type typ, struct node *left, struct node *right) {
    struct node *node = new_node();
    node->typ = typ;
    node->token = token;
    node->left = left;
    node->right = right;
    return node;
}

// NOLINTNEXTLINE(misc-no-recursion)
void print_node(struct node *node, int level) {
    printf("%s `%.*s`\n", node_type_name_table[node->typ], (int)node->token.len, node->token.buf);
    if (node->left != NULL) {
        printf("%*.s | L: ", level * 2, " ");
        print_node(node->left, level + 1);
    }

    if (node->right != NULL) {
        printf("%*.s | R: ", level * 2, " ");
        print_node(node->right, level + 1);
    }
}

struct node *parse(struct token *cur) {
    struct node *node = parse_then(cur);
    if (check_type(cur, TT_EOF)) {
        consume(cur);
    } else {
        parse_error(*cur, "expected end of file, found: `%.*s`", (int)cur->len, cur->buf);
    }

    return node;
}

struct node *parse_then(struct token *cur) {
    struct node *left = parse_appl(cur);
    for(;;) {
        if (check_type(cur, TT_EOL)) {
            struct token then_start = consume(cur);
            struct node *right = parse_appl(cur);
            left = new_binary_node(then_start, NT_THEN, left, right);
            continue;
        }

        break;
    }
    return left;
}

struct node *parse_appl(struct token *cur) {
    struct node *left = parse_primary(cur);
    for (;;) {
        struct node *right = parse_primary(cur);
        if (right == NULL) {
            break;
        }

        left = new_binary_node(left->token, NT_APPLICATION, left, right);
    }

    return left;
}

struct node *parse_primary(struct token *cur) {
    if (check_type(cur, TT_LPAR)) {
        consume(cur); // '('
        struct node *node = parse_appl(cur);
        if (!check_type(cur, TT_RPAR)) {
            parse_error(*cur, "expected ')', found: `%.*s`", (int)cur->len, cur->buf);
        }
        consume(cur); // ')'
        return node;
    }

    if (check_type(cur, TT_DEF)) {
        struct token abst_start = consume(cur); // '\'
        struct node *left = parse_id(cur);
        if (left == NULL) {
            parse_error(*cur, "was expecting an id buf found: `%.*s`", (int)cur->len, cur->buf);
        }

        for (;;) {
            struct node *right = parse_id(cur);
            if (right == NULL) {
                break;
            }

            left = new_binary_node(left->token, NT_APPLICATION, left, right);
        }

        if (!check_type(cur, TT_BODY)) {
            parse_error(*cur, "was expecting operator '.' buf found: `%.*s`", (int)cur->len, cur->buf);
        }

        consume(cur); // '.'

        return new_binary_node(abst_start, NT_ABSTRACTION, left, parse_appl(cur));
    }

    return parse_id(cur);
}

struct node *parse_id(struct token *cur) {
    if (check_type(cur, TT_ID)) {
        return new_id_node(consume(cur));
    }

    return NULL;
}