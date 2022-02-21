//
// Created by carlos on 2/21/22.
//

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "reduce.h"
#include "parse.h"

struct node *deep_copy(const struct node *node) {
    struct node *cpy = new_node();
    memcpy(cpy, node, sizeof(struct node));

    if (node->left != NULL) {
        cpy->left = deep_copy(node->left);
    }

    if (node->right != NULL) {
        cpy->right = deep_copy(node->right);
    }

    return cpy;
}

struct node *substitute(const struct node *into_expr, const char *name, struct node *with_expr) {
    // TODO: Resolve hygiene condition (maybe refactoring with_expr)
    struct node *into_expr_cpy = deep_copy(into_expr);
    if (into_expr_cpy->typ == NT_APPLICATION) {
        into_expr_cpy->left = substitute(into_expr_cpy->left, name, with_expr);
        into_expr_cpy->right = substitute(into_expr_cpy->right, name, with_expr);
    } else if (into_expr_cpy->typ == NT_DEFINITION || into_expr_cpy->typ == NT_ABSTRACTION) {
        into_expr_cpy->right = substitute(into_expr_cpy->right, name, with_expr);
    } else if (into_expr_cpy->typ == NT_ID) {
        size_t name_len = strlen(name);
        struct token view = into_expr_cpy->token;
        if (name_len == view.len && memcmp(name, view.buf, name_len) == 0) {
            *into_expr_cpy = *with_expr;
        }
    } else {
        reduce_error(into_expr->token, "cannot reduce a `then` node");
    }

    return into_expr_cpy;
}