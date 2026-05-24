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

extern polynomial_t *key_generation(size_t size, gmp_randstate_t state);

extern ciphered_t *encrypt(encoded_polynomial_t *scaled_M, polynomial_t *secret_key, mpz_t modulo, mpz_t scaling_factor, gmp_randstate_t state);

extern encoded_polynomial_t *decrypt(ciphered_t *cipher, polynomial_t *secret_key, mpz_t modulo, mpz_t scaling_factor);

extern void cipher_print(ciphered_t *c);

extern void mpz_round_to_nearest_multiple(mpz_t result, const mpz_t to_round, const mpz_t multiple);

extern void cipher_copy_A(ciphered_t *c, polynomial_t *A_to_copy);

extern void cipher_copy_B(ciphered_t *c, encoded_polynomial_t *B_to_copy);

extern polynomial_t *A_generation(size_t size, gmp_randstate_t state, mpz_t modulo);

extern polynomial_t *E_generation(size_t size);

#endif /*KEY_GENERATION_H*/