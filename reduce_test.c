//
// Created by carlos on 2/21/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "parse.h"
#include "reduce.h"

static void test_substitution_id(void **state) {
    (void) state;

    const char *src = "a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *original = parse(&current);

    struct node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = NT_ID,
            .token = {
                    .typ = TT_ID,
                    .buf = "bb",
                    .len = 2L,
            }
    };
    struct node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);

    assert_int_equal(substituted->typ, NT_ID);
    assert_int_equal(substituted->token.len, 2);
    assert_memory_equal("bb", substituted->token.buf, 2);
}

static void test_substitution_id_different_variables(void **state) {
    (void) state;

    const char *src = "a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *original = parse(&current);

    struct node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = NT_ID,
            .token = {
                    .typ = TT_ID,
                    .buf = "bb",
                    .len = 2L,
            }
    };
    struct node *substituted = substitute(original, "b", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);

    assert_int_equal(substituted->typ, NT_ID);
    assert_int_equal(substituted->token.len, 1);
    assert_memory_equal("a", substituted->token.buf, 1);
}

static void test_substitution_definition(void **state) {
    (void) state;

    const char *src = "a = a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *original = parse(&current);
    struct node *tmp0, *tmp1;

    struct node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = NT_ID,
            .token = {
                    .typ = TT_ID,
                    .buf = "bb",
                    .len = 2L,
            }
    };
    struct node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, NT_DEFINITION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_memory_equal("a", tmp0->token.buf, 1L);

    tmp1 = substituted->right;
    assert_non_null(tmp1);
    assert_memory_equal("bb", tmp1->token.buf, 2L);
}

static void test_substitution_abstraction(void **state) {
    (void) state;

    const char *src = "a => a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *original = parse(&current);
    struct node *tmp0, *tmp1;

    struct node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = NT_ID,
            .token = {
                    .typ = TT_ID,
                    .buf = "bb",
                    .len = 2L,
            }
    };
    struct node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, NT_ABSTRACTION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_memory_equal("a", tmp0->token.buf, 1L);

    tmp1 = substituted->right;
    assert_non_null(tmp1);
    assert_memory_equal("bb", tmp1->token.buf, 2L);
}

static void test_substitution_application(void **state) {
    (void) state;

    const char *src = "b a";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *original = parse(&current);
    struct node *tmp0, *tmp1;

    struct node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = NT_ID,
            .token = {
                    .typ = TT_ID,
                    .buf = "bb",
                    .len = 2L,
            }
    };
    struct node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, NT_APPLICATION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_memory_equal("b", tmp0->token.buf, 1L);

    tmp1 = substituted->right;
    assert_non_null(tmp1);
    assert_memory_equal("bb", tmp1->token.buf, 2L);
}

static void test_substitution_descending(void **state) {
    (void) state;

    const char *src = "a = a b";
    struct token start = first_token(src);
    struct token current = next_token(start);
    struct node *original = parse(&current);
    struct node *tmp0, *tmp1;

    struct node substitution = {
            .right = NULL,
            .left = NULL,
            .typ = NT_ID,
            .token = {
                    .typ = TT_ID,
                    .buf = "bb",
                    .len = 2L,
            }
    };
    struct node *substituted = substitute(original, "a", &substitution);
    assert_non_null(substituted);
    assert_ptr_not_equal(substituted, original);
    assert_int_equal(substituted->typ, NT_DEFINITION);

    tmp0 = substituted->left;
    assert_non_null(tmp0);
    assert_memory_equal("a", tmp0->token.buf, 1L);

    tmp0 = substituted->right;
    assert_non_null(tmp0);
    assert_int_equal(tmp0->typ, NT_APPLICATION);

    tmp1 = tmp0->left;
    assert_non_null(tmp1);
    assert_int_equal(tmp1->typ, NT_ID);
    assert_memory_equal("bb", tmp1->token.buf, 2L);

    tmp1 = tmp0->right;
    assert_non_null(tmp1);
    assert_int_equal(tmp1->typ, NT_ID);
    assert_memory_equal("b", tmp1->token.buf, 1L);
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