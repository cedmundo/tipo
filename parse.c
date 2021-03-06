//
// Created by carlos on 2/18/22.
//
// NOLINTBEGIN(misc-no-recursion)
#include "lex.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static_assert(AST_TYPE_COUNT == 5, "Exhaustive ast_node type name table");
static const char *ast_node_type_name_table[] = {
        [AST_TYPE_IDENTITY] = "identity",
        [AST_TYPE_DEFINITION] = "definition",
        [AST_TYPE_ABSTRACTION] = "abstraction",
        [AST_TYPE_APPLICATION] = "application",
        [AST_TYPE_THEN] = "then",
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


struct ast_node *new_ast_node() {
    return calloc(1, sizeof(struct ast_node));
}

struct ast_node *new_ast_node_id(struct token token) {
    struct ast_node *node = new_ast_node();
    node->typ = AST_TYPE_IDENTITY;
    node->token = token;
    return node;
}

struct ast_node *new_ast_node_binary(struct token token, enum ast_node_type typ, struct ast_node *left, struct ast_node *right) {
    struct ast_node *node = new_ast_node();
    node->typ = typ;
    node->token = token;
    node->left = left;
    node->right = right;
    return node;
}

void free_ast_node(struct ast_node *node) {
    if (node->left != NULL) {
        free_ast_node(node->left);
    }

    if (node->right != NULL) {
        free_ast_node(node->right);
    }

    free(node);
}

void print_ast_node(struct ast_node *node, int level) {
    if (node->typ == AST_TYPE_DEFINITION || node->typ == AST_TYPE_ABSTRACTION || node->typ == AST_TYPE_IDENTITY) {
        printf("%s `%.*s`\n", ast_node_type_name_table[node->typ], (int)node->token.len, node->token.buf);
    } else {
        printf("%s at %lu:%lu\n", ast_node_type_name_table[node->typ], node->token.row, node->token.col);
    }

    if (node->left != NULL) {
        printf("%*.s | L: ", level * 2, " ");
        print_ast_node(node->left, level + 1);
    }

    if (node->right != NULL) {
        printf("%*.s | R: ", level * 2, " ");
        print_ast_node(node->right, level + 1);
    }
}

// src ::= exprs EOF
struct ast_node *parse(struct token *cur) {
    struct ast_node *node = parse_exprs(cur);
    expect(cur, TT_EOF);
    return node;
}

// exprs ::= definition | definition { EOL definition }
struct ast_node *parse_exprs(struct token *cur) {
    struct ast_node *left = parse_definition_expr(cur);
    for(;;) {
        if (check_type(cur, TT_EOL)) {
            struct token then_start = consume(cur);
            struct ast_node *right = parse_definition_expr(cur);
            left = new_ast_node_binary(then_start, AST_TYPE_THEN, left, right);
            continue;
        }

        break;
    }
    return left;
}

// definition ::= abstraction | abstraction { '=' abstraction }
struct ast_node *parse_definition_expr(struct token *cur) {
    // TODO: Test associativity
    struct ast_node *left = parse_abstraction_expr(cur);
    for (;;) {
        if (check_type(cur, TT_ASSIGN)) {
            struct token def_start = consume(cur); // '='
            struct ast_node *right = parse_definition_expr(cur);
            left = new_ast_node_binary(def_start, AST_TYPE_DEFINITION, left, right);
            continue;
        }

        break;
    }

    return left;
}

// abstraction ::= application | application { ':' application }
struct ast_node *parse_abstraction_expr(struct token *cur) {
    struct ast_node *left = parse_application_expr(cur);
    for (;;) {
        if (check_type(cur, TT_LAMBDA)) {
            struct token lambda_start = consume(cur); // ':'
            struct ast_node *right = parse_abstraction_expr(cur);
            left = new_ast_node_binary(lambda_start, AST_TYPE_ABSTRACTION, left, right);
            continue;
        }

        break;
    }

    return left;
}

// application ::= primary | primary { primary }
struct ast_node *parse_application_expr(struct token *cur) {
    struct ast_node *left = parse_primary_expr(cur);
    for (;;) {
        struct ast_node *right = parse_primary_expr(cur);
        if (right == NULL) {
            break;
        }

        left = new_ast_node_binary(left->token, AST_TYPE_APPLICATION, left, right);
    }

    return left;
}

// primary ::= '(' definition ')' | id
struct ast_node *parse_primary_expr(struct token *cur) {
    if (check_type(cur, TT_LPAR)) {
        consume(cur); // '('
        struct ast_node *prec_climbing = parse_definition_expr(cur);
        expect(cur, TT_RPAR); // ')'
        return prec_climbing;
    }

    return parse_id(cur);
}

struct ast_node *parse_id(struct token *cur) {
    if (check_type(cur, TT_ID)) {
        return new_ast_node_id(consume(cur));
    }

    return NULL;
}
// NOLINTEND(misc-no-recursion)