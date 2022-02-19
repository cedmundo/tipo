//
// Created by carlos on 2/19/22.
//

#ifndef TIPO_UTF8_H
#define TIPO_UTF8_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

uint8_t enc_cp_to_bytes(uint32_t codepoint, char *dest);
uint8_t dec_bytes_to_cp(const char src[4], uint32_t *codepoint);

#endif //TIPO_UTF8_H
