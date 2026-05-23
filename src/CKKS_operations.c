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

ciphered_t *ckks_mult_cipher_plain(const ciphered_t *c, encoded_polynomial_t *p, int64_t modulo, int64_t scaling_factor)
{
    ciphered_t *mult = malloc(sizeof(ciphered_t));
    if (mult == NULL)
    {
        fprintf(stderr, "failed to allocate");
        return NULL;
    }
    // Rescaling the plain text to same scale as cipher text (MODIFIED ON PLACE WATCHOUT)
    encoded_pol_scalar_mult(p, scaling_factor);
    encoded_polynomial_t *mult_A = encoded_pol_mult_polynomial_modulo(p, c->A, modulo);
    if (mult_A == NULL)
    {
        return NULL;
    }
    mult->A = encoded_to_polynomial(mult_A);
    if (mult->A == NULL)
    {
        free(mult);
        return NULL;
    }
    mult->B = encoded_pol_mult_modulo_no_rescaling(c->B, p, modulo);
    if (mult->B == NULL)
    {
        polynomial_free(mult->A);
        free(mult);
        return NULL;
    }

    // Reduce multiplicative level by 1
    div_and_round_polynomial(mult->A, modulo);
    div_and_round_encoded_polynomial(mult->B, modulo);
    encoded_pol_free(mult_A);
    return mult;
}

// Function used to pick a large g in modulus switch below (Q_L)^2
int64_t random_power_of_2_below(int64_t A)
{

    int max_exp = 0;
    while ((int64_t)1 << (max_exp + 1) < A)
        max_exp++;

    int exp = max_exp / 2 + rand() % (max_exp / 2 + 1); // return an random exp between max_exp / 2 and max_exp
    return (int64_t)1 << exp;                           // 2^exp;
}

// Should be pre computed at the beginning with every modulo levels : encryption of g*(S^2) mod gq.
ciphered_t *evk_g(polynomial_t *secret_key, int64_t current_modulo, int64_t g)
{
    ciphered_t *res = cipher_init(secret_key->degree);
    if (res == NULL)
    {
        return NULL;
    }
    polynomial_t *A_prime = A_generation(secret_key->degree); // MAsk generation
    if (A_prime == NULL)
    {
        return NULL;
    }
    cipher_copy_A(res, A_prime);
    polynomial_t *E_prime = E_generation(secret_key->degree); // Error generation
    if (E_prime == NULL)
    {
        return NULL;
    }
    polynomial_t *prod = polynomial_mult_modulo(A_prime, secret_key, g * current_modulo); // A * S
    if (prod == NULL)
    {
        return NULL;
    }

    polynomial_scalar_mult(prod, -1); // -(A * S)
    polynomial_t *S_squared = polynomial_mult(secret_key, secret_key);
    if (S_squared == NULL)
    {
        return NULL;
    }
    polynomial_scalar_mult(S_squared, g); // g*S^2

    polynomial_add_modulo(S_squared, E_prime, g * current_modulo); // g*S^2 + E

    polynomial_add_modulo(S_squared, prod, g * current_modulo); // g*S^2 + E -(A * S)

    // copying S_squared into res->B
    for (size_t i = 0; i < S_squared->degree; i++)
    {
        res->B->coeffs[i] = S_squared->coeffs[i];
    }

    polynomial_free(A_prime);
    polynomial_free(E_prime);
    polynomial_free(prod);
    polynomial_free(S_squared);
    return res;
}

ciphered_t *ckks_mult_cipher_cipher(ciphered_t *c1, ciphered_t *c2, int64_t modulo,  ciphered_t *evk_g_precomputed, int64_t g)
{

    ciphered_t *mult = cipher_init(c1->A->degree);
    // D0 = B1 * B2
    encoded_polynomial_t *D0 = encoded_pol_mult_modulo_no_rescaling(c1->B, c2->B, modulo);
    if (D0 == NULL)
    {
        return NULL;
    }

    // D1 = B1 * A2 + A1 * B2
    encoded_polynomial_t *temp1 = encoded_pol_mult_polynomial_modulo(c1->B, c2->A, modulo);
    if (temp1 == NULL)
    {
        return NULL;
    }

    encoded_polynomial_t *temp2 = encoded_pol_mult_polynomial_modulo(c2->B, c1->A, modulo);
    if (temp2 == NULL)
    {
        return NULL;
    }
    encoded_pol_add_modulo(temp1, temp2, modulo);
    polynomial_t *D1 = encoded_to_polynomial(temp1);
    if (D1 == NULL)
    {
        return NULL;
    }

    // D2 = A1 * A2
    polynomial_t *D2 = polynomial_mult_modulo(c1->A, c2->A, modulo);
    if (D2 == NULL)
    {
        return NULL;
    }

    // Relinearization
    // Peut être mettre des modulo après
    //  D2 * evk_g.A
    polynomial_t *D2_A_prod = polynomial_mult(D2, evk_g_precomputed->A);
    if (D2_A_prod == NULL)
    {
        return NULL;
    }
    // D2 * evk_g.B
    encoded_polynomial_t *D2_B_prod = encoded_pol_mult_polynomial(evk_g_precomputed->B, D2);
    if (D2_B_prod == NULL)
    {
        return NULL;
    }

    div_and_round_polynomial(D2_A_prod, g);
    div_and_round_encoded_polynomial(D2_B_prod, g);

    polynomial_add(D1, D2_A_prod);
    encoded_pol_add(D2_B_prod, D0);

    // Rescaling
    div_and_round_polynomial(D1, modulo);
    div_and_round_encoded_polynomial(D0, modulo);

    cipher_copy_A(mult, D1);
    cipher_copy_B(mult, D0);

    encoded_pol_free(D0);
    encoded_pol_free(temp1);
    encoded_pol_free(temp2);
    polynomial_free(D1);
    polynomial_free(D2);
    polynomial_free(D2_A_prod);
    encoded_pol_free(D2_B_prod);

    return mult;
}