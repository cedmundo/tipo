//
// Created by carlos on 2/21/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include <string.h>
#include "ir.h"

static void test_clone_ir_node(void **state) {
    (void) state;

    struct ir_node *left = new_ir_node_id("a", 1, 0, 0);
    struct ir_node *right = new_ir_node_id("b", 1, 0, 0);
    struct ir_node *root = new_ir_node_binary(IR_TYPE_APPLICATION, left, right, 0, 0);

    struct ir_node *cloned = clone_ir_node(root);
    assert_ptr_not_equal(root, cloned);

    assert_non_null(cloned->left);
    assert_ptr_not_equal(left, cloned->left);
    assert_string_equal(left->str, cloned->left->str);

    assert_non_null(cloned->right);
    assert_ptr_not_equal(right, cloned->right);
    assert_string_equal(right->str, cloned->right->str);
}

static bool are_ir_nodes_equal(struct ir_node *a, struct ir_node *b) {
    if (a->typ != b->typ) {
        return false;
    }

    if (a->typ == IR_TYPE_IDENTITY) {
        if (strcmp(a->str, b->str) == 0) {
            return true;
        }
    } else {
        return are_ir_nodes_equal(a->left, b->left) && are_ir_nodes_equal(a->right, b->right);
    }

    return false;
}

static void test_normalize_abstraction(void **state) {
    (void) state;

    const char *expect_src = "a => b => c => w";
    struct token expect_start = first_token(expect_src);
    struct token expect_current = next_token(expect_start);
    struct ast_node *expect_ast_node;
    struct ir_node *expect_ir_node;

    expect_ast_node = parse(&expect_current);
    expect_ir_node = new_ir_node_from_ast_node(expect_ast_node);

    const char *test_src = "a b c => w";
    struct token test_start = first_token(test_src);
    struct token test_current = next_token(test_start);
    struct ast_node *test_ast_node;
    struct ir_node *test_ir_node;

    test_ast_node = parse(&test_current);
    test_ir_node = new_ir_node_from_ast_node(test_ast_node);
    assert_true(are_ir_nodes_equal(expect_ir_node, test_ir_node));
}

static void test_normalize_abstraction_with_definition(void **state) {
    (void) state;

    const char *expect_src = "f = a => b => c => w";
    struct token expect_start = first_token(expect_src);
    struct token expect_current = next_token(expect_start);
    struct ast_node *expect_ast_node;
    struct ir_node *expect_ir_node;

    expect_ast_node = parse(&expect_current);
    expect_ir_node = new_ir_node_from_ast_node(expect_ast_node);

    const char *test_src = "f a b c = w";
    struct token test_start = first_token(test_src);
    struct token test_current = next_token(test_start);
    struct ast_node *test_ast_node;
    struct ir_node *test_ir_node;

    test_ast_node = parse(&test_current);
    test_ir_node = new_ir_node_from_ast_node(test_ast_node);
    assert_true(are_ir_nodes_equal(expect_ir_node, test_ir_node));
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_clone_ir_node),
            cmocka_unit_test(test_normalize_abstraction),
            cmocka_unit_test(test_normalize_abstraction_with_definition),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}