//
// Created by carlos on 2/18/22.
//

#ifndef TIPO_PARSE_H
#define TIPO_PARSE_H
#include "lex.h"

enum node_type {
    NT_ID,
    NT_ABSTRACTION,
    NT_APPLICATION,
    NT_THEN,
    NT_COUNT,
};

struct node {
    struct node *left;
    struct node *right;
    struct token token;
    enum node_type typ;
};

struct node *new_node();
struct node *new_id_node(struct token token);
struct node *new_binary_node(struct token token, enum node_type typ, struct node *left, struct node *right);

void print_node(struct node *node, int level);

struct node *parse(struct token *cur);
struct node *parse_then(struct token *cur);
struct node *parse_appl(struct token *cur);
struct node *parse_primary(struct token *cur);
struct node *parse_id(struct token *cur);

#define parse_error(t, msg, ...) do{ printf("%lu:%lu: " msg "\n", (t).row, (t).col, ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_PARSE_H
