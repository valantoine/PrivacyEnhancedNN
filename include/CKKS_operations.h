#ifndef CKKS_OPERATIONS_H
#define CKKS_OPERATIONS_H
#include "encrypt.h"

extern void ckks_add_cipher_cipher(ciphered_t *sum, const ciphered_t *c1, const ciphered_t *c2, mpz_t modulo);

extern void ckks_add_cipher_plain(ciphered_t *sum, const ciphered_t *c, encoded_polynomial_t *p, mpz_t modulo, mpz_t scaling_factor);

extern ciphered_t *ckks_mult_cipher_plain(const ciphered_t *c, encoded_polynomial_t *p, mpz_t modulo, mpz_t scaling_factor);

extern ciphered_t *evk_g(polynomial_t *secret_key, mpz_t current_modulo, mpz_t g, gmp_randstate_t state);

extern void big_power_of_2_below(mpz_t g, const mpz_t Q);

extern ciphered_t *ckks_mult_cipher_cipher(ciphered_t *c1, ciphered_t *c2, mpz_t modulo, ciphered_t *evk_g_precomputed, mpz_t g, mpz_t scaling_factor);


#endif /*CKKS_OPERATIONS_H*/