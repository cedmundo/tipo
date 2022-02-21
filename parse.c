//
// Created by carlos on 2/18/22.
//
// NOLINTBEGIN(misc-no-recursion)
#include "lex.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static_assert(NT_COUNT == 5, "Exhaustive node type name table");
static const char *node_type_name_table[] = {
        [NT_ID] = "identity",
        [NT_DEFINITION] = "definition",
        [NT_ABSTRACTION] = "abstraction",
        [NT_APPLICATION] = "application",
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

struct token expect(struct token *cur, enum token_type typ) {
    if (!check_type(cur, typ)) {
        parse_error(*cur, "expected end of file, found: `%.*s`", (int)cur->len, cur->buf);
    }

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

void free_node(struct node *node) {
    if (node->left != NULL) {
        free_node(node->left);
    }

    if (node->right != NULL) {
        free_node(node->right);
    }

    free(node);
}

void print_node(struct node *node, int level) {
    if (node->typ == NT_DEFINITION || node->typ == NT_ABSTRACTION || node->typ == NT_ID) {
        printf("%s `%.*s`\n", node_type_name_table[node->typ], (int)node->token.len, node->token.buf);
    } else {
        printf("%s at %lu:%lu\n", node_type_name_table[node->typ], node->token.row, node->token.col);
    }

    if (node->left != NULL) {
        printf("%*.s | L: ", level * 2, " ");
        print_node(node->left, level + 1);
    }

    if (node->right != NULL) {
        printf("%*.s | R: ", level * 2, " ");
        print_node(node->right, level + 1);
    }
}

// src ::= exprs EOF
struct node *parse(struct token *cur) {
    struct node *node = parse_exprs(cur);
    expect(cur, TT_EOF);
    return node;
}

// exprs ::= definition | definition { EOL definition }
struct node *parse_exprs(struct token *cur) {
    struct node *left = parse_definition_expr(cur);
    for(;;) {
        if (check_type(cur, TT_EOL)) {
            struct token then_start = consume(cur);
            struct node *right = parse_definition_expr(cur);
            left = new_binary_node(then_start, NT_THEN, left, right);
            continue;
        }

        break;
    }
    return left;
}

// definition ::= abstraction | abstraction { '=' abstraction }
struct node *parse_definition_expr(struct token *cur) {
    // TODO: Test associativity
    struct node *left = parse_abstraction_expr(cur);
    for (;;) {
        if (check_type(cur, TT_ASSIGN)) {
            struct token def_start = consume(cur); // '='
            struct node *right = parse_definition_expr(cur);
            left = new_binary_node(def_start, NT_DEFINITION, left, right);
            continue;
        }

        break;
    }

    return left;
}

// abstraction ::= application | application { ':' application }
struct node *parse_abstraction_expr(struct token *cur) {
    struct node *left = parse_application_expr(cur);
    for (;;) {
        if (check_type(cur, TT_LAMBDA)) {
            struct token lambda_start = consume(cur); // ':'
            struct node *right = parse_abstraction_expr(cur);
            left = new_binary_node(lambda_start, NT_ABSTRACTION, left, right);
            continue;
        }

        break;
    }

    return left;
}

// application ::= primary | primary { primary }
struct node *parse_application_expr(struct token *cur) {
    struct node *left = parse_primary_expr(cur);
    for (;;) {
        struct node *right = parse_primary_expr(cur);
        if (right == NULL) {
            break;
        }

        left = new_binary_node(left->token, NT_APPLICATION, left, right);
    }

    return left;
}

// primary ::= '(' definition ')' | id
struct node *parse_primary_expr(struct token *cur) {
    if (check_type(cur, TT_LPAR)) {
        consume(cur); // '('
        struct node *prec_climbing = parse_definition_expr(cur);
        expect(cur, TT_RPAR); // ')'
        return prec_climbing;
    }

    return parse_id(cur);
}

struct node *parse_id(struct token *cur) {
    if (check_type(cur, TT_ID)) {
        return new_id_node(consume(cur));
    }

    return NULL;
}
// NOLINTEND(misc-no-recursion)