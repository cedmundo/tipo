//
// Created by carlos on 2/18/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "lex.h"

static void test_next_token(void **state) {
    (void) state;

    const char *program = "a abc ;; (\n) => =";
    struct token current = first_token(program);

    current = next_token(current);
    assert_int_equal(current.typ, TT_ID);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 1);
    assert_int_equal(current.col, 1);
    assert_memory_equal("a", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_ID);
    assert_int_equal(current.len, 3);
    assert_int_equal(current.row, 1);
    assert_int_equal(current.col, 3);
    assert_memory_equal("abc", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_EOL);
    assert_int_equal(current.len, 2);
    assert_int_equal(current.row, 1);
    assert_int_equal(current.col, 7);
    assert_memory_equal(";;", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_LPAR);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 1);
    assert_int_equal(current.col, 10);
    assert_memory_equal("(", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_EOL);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 1);
    assert_memory_equal("\n", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_RPAR);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 2);
    assert_memory_equal(")", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_LAMBDA);
    assert_int_equal(current.len, 2);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 4);
    assert_memory_equal("=>", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.typ, TT_ASSIGN);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 7);
    assert_memory_equal("=", current.buf, current.len);
}

static void test_ignore_comments(void **state) {
    (void) state;

    const char *program = "# A line comment \nx";
    struct token current = first_token(program);

    current = next_token(current);
    assert_int_equal(current.typ, TT_ID);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 1);
    assert_memory_equal("x", current.buf, current.len);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_next_token),
            cmocka_unit_test(test_ignore_comments),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}