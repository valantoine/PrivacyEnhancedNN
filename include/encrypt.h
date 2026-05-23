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

extern ciphered_t *cipher_init(size_t size);

extern void cipher_free(ciphered_t *c);

extern polynomial_t *key_generation(size_t size);

extern ciphered_t *encrypt(encoded_polynomial_t *scaled_M, polynomial_t *secret_key, uint64_t modulo, int64_t scaling_factor);

extern encoded_polynomial_t *decrypt(ciphered_t *cipher, polynomial_t *secret_key, uint64_t modulo, uint64_t scaling_factor);

extern void cipher_print(ciphered_t *c);

#endif /*KEY_GENERATION_H*/