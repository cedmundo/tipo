//
// Created by carlos on 2/21/22.
//

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "ir.h"
#include "reduce.h"

struct ir_node *substitute(const struct ir_node *into_expr, const char *name, struct ir_node *with_expr) {
    // TODO: Resolve hygiene condition (maybe refactoring with_expr)
    struct ir_node *into_expr_cpy = clone_ir_node(into_expr);
    if (into_expr_cpy->typ == IR_TYPE_APPLICATION) {
        into_expr_cpy->left = substitute(into_expr_cpy->left, name, with_expr);
        into_expr_cpy->right = substitute(into_expr_cpy->right, name, with_expr);
    } else if (into_expr_cpy->typ == IR_TYPE_DEFINITION || into_expr_cpy->typ == IR_TYPE_ABSTRACTION) {
        into_expr_cpy->right = substitute(into_expr_cpy->right, name, with_expr);
    } else if (into_expr_cpy->typ == IR_TYPE_IDENTITY) {
        if (strcmp(name, into_expr_cpy->str) == 0) {
            *into_expr_cpy = *with_expr;
        }
    } else {
        reduce_error(into_expr->row, into_expr->col, "cannot reduce a `then` ast_node");
    }

    return into_expr_cpy;
}