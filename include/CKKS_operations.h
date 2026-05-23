#ifndef CKKS_OPERATIONS_H
#define CKKS_OPERATIONS_H
#include "encrypt.h"

extern void ckks_add_cipher_cipher(ciphered_t *sum, const ciphered_t *c1, const ciphered_t *c2, int64_t modulo);

extern void ckks_add_cipher_plain(ciphered_t *sum, const ciphered_t *c, encoded_polynomial_t *p, int64_t modulo, int64_t scaling_factor);




#endif /*CKKS_OPERATIONS_H*/