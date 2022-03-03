//
// Created by carlos on 2/21/22.
//

#ifndef TIPO_IR_H
#define TIPO_IR_H
#include <stddef.h>
#include <stdbool.h>
#include "parse.h"

enum ir_node_type {
    IR_TYPE_IDENTITY,
    IR_TYPE_DEFINITION,
    IR_TYPE_APPLICATION,
    IR_TYPE_ABSTRACTION,
    IR_TYPE_THEN,
    IR_TYPE_COUNT
};

struct ir_node {
    struct ir_node *left;
    struct ir_node *right;
    char *str;
    size_t row;
    size_t col;
    enum ir_node_type typ;
};

struct ir_node *new_ir_node();
struct ir_node *new_ir_node_id(const char *str, size_t len, size_t row, size_t col);
struct ir_node *new_ir_node_binary(enum ir_node_type typ, struct ir_node *left, struct ir_node *right, size_t row, size_t col);
struct ir_node *clone_ir_node(struct ir_node *node);

void free_ir_node(struct ir_node *node);
void print_ir_node(struct ir_node *node, int level);

struct ir_node *normalize_abstraction(struct ir_node *root);
struct ir_node *new_ir_node_from_ast_node(struct ast_node *node);

#define ir_error(row, col, msg, ...) do{ printf("%lu:%lu: " msg "\n", (row), (col), ##__VA_ARGS__); exit(1); }while(0)
#endif //TIPO_IR_H
