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

    node = parse_primary(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ID);
    assert_int_equal(node->token.typ, TT_ID);
    assert_int_equal(node->token.len, 3);
    assert_memory_equal("abc", node->token.buf, node->token.len);

    node = parse_primary(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ID);
    assert_int_equal(node->token.typ, TT_ID);
    assert_int_equal(node->token.len, 3);
    assert_memory_equal("cba", node->token.buf, node->token.len);
}

static void test_parse_precedence(void **state) {
    const char *src = "(abc)";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node;

    node = parse_primary(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ID);
    assert_int_equal(node->token.typ, TT_ID);
    assert_int_equal(node->token.len, 3);
    assert_memory_equal("abc", node->token.buf, node->token.len);
}

static void test_parse_abstraction(void **state) {
    const char *src = "\\I x = x";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *args, *body, *left, *right;

    node = parse_primary(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_ABSTRACTION);

    args = node->left;
    assert_non_null(args);
    assert_int_equal(args->typ, NT_APPLICATION);

    left = args->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.typ, TT_ID);
    assert_int_equal(left->token.len, 1);
    assert_memory_equal("I", left->token.buf, left->token.len);

    right = args->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.typ, TT_ID);
    assert_int_equal(right->token.len, 1);
    assert_memory_equal("x", right->token.buf, right->token.len);

    body = node->right;
    assert_non_null(body);
    assert_int_equal(body->typ, NT_ID);
    assert_int_equal(body->token.len, 1);
    assert_memory_equal("x", body->token.buf, body->token.len);
}

static void test_parse_application(void **state) {
    const char *src = "a b c";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse_appl(&current);
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
}

static void test_parse_then(void **state) {
    (void) state;

    const char *src = "abc ; cba";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *node, *left, *right;

    node = parse_then(&current);
    assert_non_null(node);
    assert_int_equal(node->typ, NT_THEN);

    left = node->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ID);
    assert_int_equal(left->token.typ, TT_ID);
    assert_int_equal(left->token.len, 3);
    assert_memory_equal("abc", left->token.buf, left->token.len);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
    assert_int_equal(right->token.typ, TT_ID);
    assert_int_equal(right->token.len, 3);
    assert_memory_equal("cba", right->token.buf, right->token.len);
}

static void test_parse(void **state) {
    (void) state;

    const char *src = "\\I a = a; I I; I";
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
    assert_int_equal(right->typ, NT_APPLICATION);

    left = left->left;
    assert_non_null(left);
    assert_int_equal(left->typ, NT_ABSTRACTION);

    right = node->right;
    assert_non_null(right);
    assert_int_equal(right->typ, NT_ID);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_id),
            cmocka_unit_test(test_parse_precedence),
            cmocka_unit_test(test_parse_abstraction),
            cmocka_unit_test(test_parse_application),
            cmocka_unit_test(test_parse_then),
            cmocka_unit_test(test_parse),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
