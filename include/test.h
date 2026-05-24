#ifndef TEST_H
#define TEST_H
#include "encrypt.h"
#include "CKKS_encoding.h"
#include "CKKS_operations.h"

extern void test_encode_decode();

extern void test_add_encode_decode();

extern void test_mult_encode_decode();

extern void test_encrypt_decrypt();

extern void test_encrypt_add_cipher();

extern void test_encrypt_add_plain();

extern void test_encrypt_mult_plain();

extern void test_encrypt_mult_cipher();

#endif /*TEST_H*/