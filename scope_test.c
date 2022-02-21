//
// Created by carlos on 2/20/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "scope.h"

static void test_undefined(void **state) {
    (void) state;

    struct scope *scope = new_scope();
    struct scope *child = fork_scope(scope);
    struct bind *bind = get_bind(child, (struct token) {.buf="a", .len=1L}, true);
    assert_null(bind);
    scope_free(scope);
}

static void test_define_and_lookup(void **state) {
    (void) state;

    struct scope *root = new_scope();
    struct scope *scope;

    scope = def_bind(root, BIND_INTRINSIC, (struct token) {.buf = "a", .len = 1L}, NULL);
    assert_ptr_equal(root, scope);

    scope = def_bind(root, BIND_INTRINSIC, (struct token) {.buf = "b", .len = 1L}, NULL);
    assert_ptr_equal(root, scope);

    struct bind *bind = get_bind(root, (struct token) { .buf = "a", .len=1L}, true);
    assert_non_null(bind);
    assert_null(bind->value);
    assert_int_equal(bind->typ, BIND_INTRINSIC);
    assert_memory_equal("a", bind->name.buf, 1L);

    scope = def_bind(root, BIND_INTRINSIC, (struct token) {.buf="a", .len=1L}, NULL);
    assert_ptr_not_equal(root, scope);

    struct bind *first = get_bind(root, (struct token){.buf="a", .len=1L}, true);
    struct bind *second = get_bind(scope, (struct token){.buf="a", .len=1L}, true);
    assert_ptr_not_equal(first, second);

    bind = get_bind(scope, (struct token) {.buf="b", .len=1L}, true);
    assert_non_null(bind);
    assert_int_equal(bind->typ, BIND_INTRINSIC);
    assert_memory_equal("b", bind->name.buf, 1L);

    scope_free(root);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_undefined),
            cmocka_unit_test(test_define_and_lookup),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}