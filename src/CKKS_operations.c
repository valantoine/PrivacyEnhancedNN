#include "CKKS_operations.h"

void ckks_add_cipher_cipher(ciphered_t *sum, const ciphered_t *c1, const ciphered_t *c2, int64_t modulo)
{
    for (size_t i = 0; i < sum->A->degree; i++)
    {
        sum->A->coeffs[i] = (c1->A->coeffs[i] + c2->A->coeffs[i]) % modulo;
        sum->B->coeffs[i] = (c1->B->coeffs[i] + c2->B->coeffs[i]) % modulo;
    }
}

void ckks_add_cipher_plain(ciphered_t *sum, const ciphered_t *c, encoded_polynomial_t *p, int64_t modulo, int64_t scaling_factor)
{

    // Rescaling the plain text to same scale as cipher text (MODIFIED ON PLACE WATCHOUT)
    encoded_pol_scalar_mult(p, scaling_factor);

    for (size_t i = 0; i < sum->A->degree; i++)
    {
        sum->A->coeffs[i] = c->A->coeffs[i];
        sum->B->coeffs[i] = (c->B->coeffs[i] + p->coeffs[i]) % modulo;
    }
}
