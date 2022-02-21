//
// Created by carlos on 2/18/22.
//

#ifndef TIPO_PARSE_H
#define TIPO_PARSE_H
#include "lex.h"

enum ast_node_type {
    NT_ID,
    NT_DEFINITION,
    NT_ABSTRACTION,
    NT_APPLICATION,
    NT_THEN,
    NT_COUNT,
};

struct ast_node {
    struct ast_node *left;
    struct ast_node *right;
    struct token token;
    enum ast_node_type typ;
};

struct ast_node *new_ast_node();
struct ast_node *new_ast_id_node(struct token token);
struct ast_node *new_ast_binary_node(struct token token, enum ast_node_type typ, struct ast_node *left, struct ast_node *right);

void free_ast_node(struct ast_node *node);
void print_ast_node(struct ast_node *node, int level);

struct ast_node *parse(struct token *cur);
struct ast_node *parse_exprs(struct token *cur);
struct ast_node *parse_definition_expr(struct token *cur);
struct ast_node *parse_abstraction_expr(struct token *cur);
struct ast_node *parse_application_expr(struct token *cur);
struct ast_node *parse_primary_expr(struct token *cur);
struct ast_node *parse_id(struct token *cur);

#define parse_error(t, msg, ...) do{ printf("%lu:%lu: " msg "\n", (t).row, (t).col, ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_PARSE_H
