//
// Created by carlos on 2/18/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "lex.h"
#include "parse.h"

static void test_parse_id(void **state) {
    (void) state;

    const char *src = "abc cba";
    struct token start = first_token(src);
    struct token current = next_token(start);

    struct node *node;

    node = parse_id(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ID);
    assert_int_equal(node->token.typ, TT_ID);
    assert_int_equal(node->token.len, 3);
    assert_memory_equal("abc", node->token.buf, node->token.len);
    free_node(node);

    node = parse_id(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ID);
    assert_int_equal(node->token.typ, TT_ID);
    assert_int_equal(node->token.len, 3);
    assert_memory_equal("cba", node->token.buf, node->token.len);
    free_node(node);
}

static void test_parse_primary(void **state) {
    (void) state;

    const char *src = "(abc)";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node;

    node = parse_primary_expr(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ID);
    assert_int_equal(node->token.typ, TT_ID);
    assert_int_equal(node->token.len, 3);
    assert_memory_equal("abc", node->token.buf, node->token.len);

    free_node(node);
}

static void test_parse_application(void **state) {
    (void) state;

    const char *src = "a b c";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse_application_expr(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_APPLICATION);

    left = node->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_APPLICATION);

    right = left->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("b", right->token.buf, right->token.len);

    left = left->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("a", left->token.buf, left->token.len);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("c", right->token.buf, right->token.len);

    free_node(node);
}

static void test_parse_abstraction(void **state) {
    (void) state;

    const char *src = "a => b => c";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse_abstraction_expr(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ABSTRACTION);

    left = node->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("a", left->token.buf, left->token.len);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ABSTRACTION);

    left = right->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("b", left->token.buf, left->token.len);

    right = right->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("c", right->token.buf, right->token.len);

    free_node(node);
}

static void test_parse_definition(void **state) {
    (void) state;

    const char *src = "a = b = c";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse_definition_expr(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_DEFINITION);

    left = node->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("a", left->token.buf, left->token.len);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_DEFINITION);

    left = right->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("b", left->token.buf, left->token.len);

    right = right->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("c", right->token.buf, right->token.len);

    free_node(node);
}

static void test_parse_exprs(void **state) {
    (void) state;

    const char *src = "a ; b ; c";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse_exprs(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_THEN);

    left = node->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_THEN);

    right = left->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("b", right->token.buf, right->token.len);

    left = left->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("a", left->token.buf, left->token.len);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("c", right->token.buf, right->token.len);

    free_node(node);
}

static void test_parse(void **state) {
    (void) state;

    const char *src = "a ; b ; c";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_THEN);

    left = node->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_THEN);

    right = left->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("b", right->token.buf, right->token.len);

    left = left->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("a", left->token.buf, left->token.len);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("c", right->token.buf, right->token.len);

    free_node(node);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_id),
            cmocka_unit_test(test_parse_primary),
            cmocka_unit_test(test_parse_application),
            cmocka_unit_test(test_parse_abstraction),
            cmocka_unit_test(test_parse_definition),
            cmocka_unit_test(test_parse_exprs),
            cmocka_unit_test(test_parse),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
