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

    const char *program = "a abc ;; (\n) . \\";
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
    assert_int_equal(current.typ, TT_BODY);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 4);
    assert_memory_equal(".", current.buf, current.len);

    current = next_token(current);
    assert_int_equal(current.typ, TT_DEF);
    assert_int_equal(current.len, 1);
    assert_int_equal(current.row, 2);
    assert_int_equal(current.col, 6);
    assert_memory_equal("\\", current.buf, current.len);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_next_token),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}