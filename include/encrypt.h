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

extern int64_t round_to_nearest_multiple(int64_t to_round, int64_t multiple);

extern void cipher_copy_A(ciphered_t *c, polynomial_t *A_to_copy);

extern void cipher_copy_B(ciphered_t *c, encoded_polynomial_t *B_to_copy);

extern polynomial_t *A_generation(size_t size);

extern polynomial_t *E_generation(size_t size);

#endif /*KEY_GENERATION_H*/