//
// Created by carlos on 2/21/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "ir.h"

static void test_dummy(void **state) {
    (void) state;

    assert_int_equal(1, 1);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_dummy),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}