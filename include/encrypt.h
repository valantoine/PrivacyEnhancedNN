#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "encoding_misc.h"
#include "CKKS_encoding.h"
#include "polynomial_operations.h"
extern double gauss(void);

typedef struct
{
    polynomial_t *A;
    encoded_polynomial_t *B;
} ciphered_t;

extern void test_encrypt_decrypt();

#endif /*KEY_GENERATION_H*/