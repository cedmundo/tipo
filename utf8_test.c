//
// Created by carlos on 2/19/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "utf8.h"

static void test_enc_cp_to_bytes(void **state) {
    (void) state;

    char output[5] = {0};
    assert_int_equal(enc_cp_to_bytes(0x0045, output), 1);
    assert_string_equal(output, "E");

    assert_int_equal(enc_cp_to_bytes(0x01C2, output), 2);
    assert_string_equal(output, "ǂ");

    assert_int_equal(enc_cp_to_bytes(0x3297, output), 3);
    assert_string_equal(output, "㊗");

    assert_int_equal(enc_cp_to_bytes(0x1F60C, output), 4);
    assert_string_equal(output, "😌");
}

static void test_dec_bytes_to_cp(void **state){
    (void) state;

    uint32_t codepoint = 0L;
    assert_int_equal(dec_bytes_to_cp("E", &codepoint), 1);
    assert_int_equal(codepoint, 0x0045);

    assert_int_equal(dec_bytes_to_cp("ǂ", &codepoint), 2);
    assert_int_equal(codepoint, 0x01C2);

    assert_int_equal(dec_bytes_to_cp("㊗", &codepoint), 3);
    assert_int_equal(codepoint, 0x3297);

    assert_int_equal(dec_bytes_to_cp("😌", &codepoint), 4);
    assert_int_equal(codepoint, 0x1F60C);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_enc_cp_to_bytes),
            cmocka_unit_test(test_dec_bytes_to_cp),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}