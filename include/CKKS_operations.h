#ifndef CKKS_OPERATIONS_H
#define CKKS_OPERATIONS_H
#include "encrypt.h"

extern void ckks_add_cipher_cipher(ciphered_t *sum, const ciphered_t *c1, const ciphered_t *c2, int64_t modulo);

extern void ckks_add_cipher_plain(ciphered_t *sum, const ciphered_t *c, encoded_polynomial_t *p, int64_t modulo, int64_t scaling_factor);

extern ciphered_t *ckks_mult_cipher_plain(const ciphered_t *c, encoded_polynomial_t *p, int64_t modulo, int64_t scaling_factor);

extern ciphered_t *ckks_mult_cipher_cipher(ciphered_t *c1, ciphered_t *c2, int64_t modulo,  ciphered_t *evk_g_precomputed, int64_t g);

extern ciphered_t *evk_g(polynomial_t *secret_key, int64_t current_modulo, int64_t g);

extern int64_t random_power_of_2_below(int64_t A);

#endif /*CKKS_OPERATIONS_H*/