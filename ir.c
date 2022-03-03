//
// Created by carlos on 2/21/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include "ir.h"

static_assert(IR_TYPE_COUNT == 5, "Exhaustive ir_node type name table");
static const char *ir_node_type_name_table[] = {
        [IR_TYPE_IDENTITY] = "identity",
        [IR_TYPE_DEFINITION] = "definition",
        [IR_TYPE_ABSTRACTION] = "abstraction",
        [IR_TYPE_APPLICATION] = "application",
        [IR_TYPE_THEN] = "then",
};

struct ir_node *new_ir_node() {
    return calloc(1, sizeof(struct ir_node));
}

struct ir_node *new_ir_node_id(const char *str, size_t len, size_t row, size_t col) {
    struct ir_node *node = new_ir_node();
    node->typ = IR_TYPE_IDENTITY;
    node->str = strndup(str, len);
    node->row = row;
    node->col = col;
    return node;
}

struct ir_node *
new_ir_node_binary(enum ir_node_type typ, struct ir_node *left, struct ir_node *right, size_t row, size_t col) {
    struct ir_node *node = new_ir_node();
    node->typ = typ;
    node->left = left;
    node->right = right;
    node->row = row;
    node->col = col;
    return node;
}

struct ir_node *clone_ir_node(struct ir_node *node) {
    struct ir_node *new_node = new_ir_node();
    memcpy(new_node, node, sizeof(struct ir_node));
    if (new_node->left != NULL) {
        new_node->left = clone_ir_node(new_node->left);
    }

    if (new_node->right != NULL) {
        new_node->right = clone_ir_node(new_node->right);
    }

    if (new_node->str != NULL) {
        new_node->str = strdup(new_node->str);
    }

    return new_node;
}

void free_ir_node(struct ir_node *node) {
    if (node->str != NULL) {
        free(node->str);
    }

    if (node->left != NULL) {
        free_ir_node(node->left);
    }

    if (node->right != NULL) {
        free_ir_node(node->right);
    }

    free(node);
}

void print_ir_node(struct ir_node *node, int level) {
    if (node->str != NULL) {
        printf("%s `%s`\n", ir_node_type_name_table[node->typ], node->str);
    } else {
        printf("%s\n", ir_node_type_name_table[node->typ]);
    }

    if (node->left != NULL) {
        printf("%*.s | L: ", level * 2, " ");
        print_ir_node(node->left, level + 1);
    }

    if (node->right != NULL) {
        printf("%*.s | R: ", level * 2, " ");
        print_ir_node(node->right, level + 1);
    }
}

#define push(s, v) do{ stack_##s[stack_top_##s++] = (v); }while(0)
#define top(s) (stack_##s[stack_top_##s-1])
#define pop(s) do{ stack_##s[stack_top_##s--] = NULL; }while(0)
#define empty(s) (stack_top_##s <= 0)

struct ir_node *normalize_abstraction(struct ir_node *root) {
    struct ir_node *stack_transversal[100] = {NULL};
    int32_t stack_top_transversal = 0L;

    if (root->left != NULL && root->left->typ == IR_TYPE_IDENTITY) {
        // already normalized
        return root;
    }

    struct ir_node *build_cur = NULL;
    struct ir_node *build_root = NULL;
    bool is_definition = root->typ == IR_TYPE_DEFINITION;
    bool final_abstraction = false;

    struct ir_node *cur = root;
    while (cur != NULL || !empty(transversal)) {
        // Reach the left most node of cur node
        while (cur != NULL) {
            // Place pointer to a tree node on the stack before
            // transversing the node's left subtree
            push(transversal, cur);
            cur = cur->left;
        }

        // Current must be NULL at this point
        cur = top(transversal);
        pop(transversal);

        if (build_cur == NULL) {
            build_cur = new_ir_node_binary(is_definition ? IR_TYPE_DEFINITION : IR_TYPE_ABSTRACTION, cur, NULL,
                                           cur->row, cur->col);
            build_root = build_cur;
        } else if ((cur->typ == IR_TYPE_ABSTRACTION && !final_abstraction) ||
                   (cur->typ == IR_TYPE_DEFINITION && !final_abstraction)) {
            final_abstraction = true;
        } else if (cur->typ == IR_TYPE_IDENTITY && !final_abstraction) {
            build_cur->right = new_ir_node_binary(IR_TYPE_ABSTRACTION, cur, NULL, cur->row, cur->col);
            build_cur = build_cur->right;
        } else if (cur->typ == IR_TYPE_IDENTITY && final_abstraction) {
            build_cur->right = cur;
        }

        // We have visited the node and its left subtree. Now it's the right subtree's turn
        cur = cur->right;
    }

    return build_root;
}

struct ir_node *new_ir_node_from_ast_node(struct ast_node *node) {
    switch (node->typ) {
        case AST_TYPE_IDENTITY:
            return new_ir_node_id(node->token.buf, node->token.len, node->token.row, node->token.col);
        case AST_TYPE_DEFINITION:
        case AST_TYPE_ABSTRACTION:
            return normalize_abstraction(
                    new_ir_node_binary(
                            node->typ == AST_TYPE_DEFINITION ? IR_TYPE_DEFINITION : IR_TYPE_ABSTRACTION,
                            new_ir_node_from_ast_node(node->left),
                            new_ir_node_from_ast_node(node->right),
                            node->token.row,
                            node->token.col
                    )
            );
        case AST_TYPE_APPLICATION:
            return new_ir_node_binary(IR_TYPE_APPLICATION,
                                      new_ir_node_from_ast_node(node->left),
                                      new_ir_node_from_ast_node(node->right),
                                      node->token.row,
                                      node->token.col);
        case AST_TYPE_THEN:
            return new_ir_node_binary(IR_TYPE_THEN,
                                      new_ir_node_from_ast_node(node->left),
                                      new_ir_node_from_ast_node(node->right),
                                      node->token.row,
                                      node->token.col);
        default:
            break;
    }

    ir_error(node->token.row, node->token.col, "unrecognized ast_node to transform into ir_node");
}
