//
// Created by carlos on 2/21/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "parse.h"
#include "ir.h"
#include "reduce.h"

static void test_substitution_id(void **state) {
    (void) state;

    const char *src = "a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct ast_node *original_ast = parse(&current);
    struct ir_node *original = new_ir_node_from_ast_node(original_ast);

    struct ir_node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = IR_TYPE_IDENTITY,
            .str = "bb",
    };
    struct ir_node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);

    assert_int_equal(substituted->typ, IR_TYPE_IDENTITY);
    assert_string_equal("bb", substituted->str);
}

static void test_substitution_id_different_variables(void **state) {
    (void) state;

    const char *src = "a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct ast_node *original_ast = parse(&current);
    struct ir_node *original = new_ir_node_from_ast_node(original_ast);

    struct ir_node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = IR_TYPE_IDENTITY,
            .str = "bb",
    };
    struct ir_node *substituted = substitute(original, "b", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);

    assert_int_equal(substituted->typ, IR_TYPE_IDENTITY);
    assert_string_equal("a", substituted->str);
}

static void test_substitution_definition(void **state) {
    (void) state;

    const char *src = "a = a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct ast_node *original_ast = parse(&current);
    struct ir_node *original = new_ir_node_from_ast_node(original_ast);
    struct ir_node *tmp0, *tmp1;

    struct ir_node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = IR_TYPE_IDENTITY,
            .str = "bb",
    };
    struct ir_node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, IR_TYPE_DEFINITION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_string_equal("a", tmp0->str);

    tmp1 = substituted->right;
    assert_non_null(tmp1);
    assert_string_equal("bb", tmp1->str);
}

static void test_substitution_abstraction(void **state) {
    (void) state;

    const char *src = "a => a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct ast_node *original_ast = parse(&current);
    struct ir_node *original = new_ir_node_from_ast_node(original_ast);
    struct ir_node *tmp0, *tmp1;

    struct ir_node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = IR_TYPE_IDENTITY,
            .str = "bb",
    };
    struct ir_node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, IR_TYPE_ABSTRACTION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_string_equal("a", tmp0->str);

    tmp1 = substituted->right;
    assert_non_null(tmp1);
    assert_string_equal("bb", tmp1->str);
}

static void test_substitution_application(void **state) {
    (void) state;

    const char *src = "b a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct ast_node *original_ast = parse(&current);
    struct ir_node *original = new_ir_node_from_ast_node(original_ast);
    struct ir_node *tmp0, *tmp1;

    struct ir_node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = IR_TYPE_IDENTITY,
            .str = "bb",
    };
    struct ir_node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, IR_TYPE_APPLICATION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_string_equal("b", tmp0->str);

    tmp1 = substituted->right;
    assert_non_null(tmp1);
    assert_string_equal("bb", tmp1->str);
}

static void test_substitution_descending(void **state) {
    (void) state;

    const char *src = "a = a b";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct ast_node *original_ast = parse(&current);
    struct ir_node *original = new_ir_node_from_ast_node(original_ast);
    struct ir_node *tmp0, *tmp1;

    struct ir_node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = IR_TYPE_IDENTITY,
            .str = "bb",
    };
    struct ir_node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, IR_TYPE_DEFINITION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_string_equal("a", tmp0->str);

    tmp0 = substituted->right;
    assert_non_null(tmp0);
    assert_int_equal(tmp0->typ, IR_TYPE_APPLICATION);

    tmp1 = tmp0->left;
    assert_non_null(tmp1);
    assert_int_equal(tmp1->typ, IR_TYPE_IDENTITY);
    assert_string_equal("bb", tmp1->str);

    tmp1 = tmp0->right;
    assert_non_null(tmp1);
    assert_int_equal(tmp1->typ, IR_TYPE_IDENTITY);
    assert_string_equal("b", tmp1->str);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_substitution_id),
            cmocka_unit_test(test_substitution_id_different_variables),
            cmocka_unit_test(test_substitution_definition),
            cmocka_unit_test(test_substitution_abstraction),
            cmocka_unit_test(test_substitution_application),
            cmocka_unit_test(test_substitution_descending),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}