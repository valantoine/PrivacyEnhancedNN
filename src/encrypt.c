#include "encrypt.h"

polynomial_t *key_generation(size_t size, gmp_randstate_t state)
{
    polynomial_t *key = polynomial_init(size);
    if (key == NULL)
    {
        return NULL;
    }
    mpz_t random_interval;
    mpz_init_set_ui(random_interval, 3);

    for (size_t i = 0; i < size; i++)
    {
        mpz_urandomm(key->coeffs[i], state, random_interval); // random number between 0 and 2
        mpz_sub_ui(key->coeffs[i], key->coeffs[i], 1);
    }
    mpz_clear(random_interval);
    return key;
}

polynomial_t *A_generation(size_t size, gmp_randstate_t state, mpz_t modulo)
{
    polynomial_t *A = polynomial_init(size);
    if (A == NULL)
    {
        return NULL;
    }
    mpz_t half;
    mpz_init(half);
    mpz_tdiv_q_2exp(half, modulo, 1); // half = modulo / 2, truncated

    for (size_t i = 0; i < size; i++)
    {
        mpz_urandomm(A->coeffs[i], state, modulo); // random number between 0 and 2
        mpz_sub(A->coeffs[i], A->coeffs[i], half);
    }
    mpz_clear(half);
    return A;
}

// Gaussian distrib function used to make error E (found online)
double gauss(void)
{
    double x = (double)rand() / RAND_MAX,
           y = (double)rand() / RAND_MAX,
           z = sqrt(-2 * log(x)) * cos(2 * M_PI * y);
    return z;
}

polynomial_t *E_generation(size_t size)
{
    polynomial_t *E = polynomial_init(size);
    if (E == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        mpz_set_d(E->coeffs[i], round(gauss()));
    }
    return E;
}

ciphered_t *cipher_init(size_t size)
{
    ciphered_t *c = malloc(sizeof(ciphered_t));
    if (c == NULL)
    {
        fprintf(stderr, "Failed to alloc");
        return NULL;
    }
    c->A = polynomial_init(size);
    if (c->A == NULL)
    {
        return NULL;
    }
    c->B = encoded_pol_init(size);
    if (c->B == NULL)
    {
        return NULL;
    }
    return c;
}

void cipher_init_in_place(ciphered_t *c, size_t size)
{
    c->A = polynomial_init(size);
    if (c->A == NULL)
    {
        return;
    }
    c->B = encoded_pol_init(size);
    if (c->B == NULL)
    {
        polynomial_free(c->A);
        return;
    }
}

void cipher_free(ciphered_t *c)
{
    polynomial_free(c->A);
    encoded_pol_free(c->B);
    free(c);
}

void cipher_copy_A(ciphered_t *c, polynomial_t *A_to_copy)
{
    for (size_t i = 0; i < A_to_copy->degree; i++)
    {
        mpz_set(c->A->coeffs[i], A_to_copy->coeffs[i]);
    }
    c->A->degree = A_to_copy->degree;
}

void cipher_copy_B(ciphered_t *c, encoded_polynomial_t *B_to_copy)
{
    for (size_t i = 0; i < B_to_copy->size; i++)
    {
        mpz_set(c->B->coeffs[i], B_to_copy->coeffs[i]);
    }
    c->B->size = B_to_copy->size;
}

void cipher_print(ciphered_t *c)
{
    fprintf(stdout, "Ciphered : \n A = ");
    polynomial_print(c->A);
    fprintf(stdout, "B = ");
    encoded_polynomial_print(c->B);
}

// Modulo needs to be way bigger
ciphered_t *encrypt(encoded_polynomial_t *scaled_M, polynomial_t *secret_key, mpz_t modulo, mpz_t scaling_factor, gmp_randstate_t state)
{
    encoded_pol_scalar_mult(scaled_M, scaling_factor);
    ciphered_t *c = cipher_init(scaled_M->size);
    if (c == NULL)
    {
        return NULL;
    }
    polynomial_t *A = A_generation(scaled_M->size, state, modulo); // MAsk generation
    if (A == NULL)
    {
        return NULL;
    }
    cipher_copy_A(c, A);
    polynomial_t *E = E_generation(scaled_M->size); // Error generation
    if (E == NULL)
    {
        return NULL;
    }
    // fprintf(stdout, "E : \n");
    // polynomial_print(E);

    polynomial_t *prod = polynomial_mult_modulo(A, secret_key, modulo); // A * S
    if (prod == NULL)
    {
        return NULL;
    }

    // Adding error in M, ATTENTION error E should not wrap modulo and every |coefficient| should be less than scaling_factor / 2
    encoded_pol_add_polynomial_modulo(scaled_M, E, modulo);

    // fprintf(stdout, "Polynome avec erreur encryption :\n ");
    // encoded_polynomial_print(scaled_M);

    // Adding secret mask, it can wrap modulo since we sub it at decryption
    mpz_t neg;
    mpz_init_set_si(neg, -1);
    polynomial_scalar_mult(prod, neg);

    encoded_pol_add_polynomial_modulo(scaled_M, prod, modulo);

    cipher_copy_B(c, scaled_M);
    polynomial_free(A);
    polynomial_free(E);
    polynomial_free(prod);
    mpz_clear(neg);
    return c;
}

void mpz_round_to_nearest_multiple(mpz_t result, const mpz_t to_round, const mpz_t multiple)
{
    mpz_t half, temp;
    mpz_init(half);
    mpz_init(temp);

    mpz_tdiv_q_2exp(half, multiple, 1); // half = multiple / 2

    if (mpz_sgn(to_round) >= 0)
    {
        mpz_add(temp, to_round, half);    // temp = to_round + multiple/2
        mpz_tdiv_q(temp, temp, multiple); // temp = (to_round + multiple/2) / multiple
        mpz_mul(result, temp, multiple);  // result = temp * multiple
    }
    else
    {
        mpz_neg(temp, to_round);          // temp = -to_round
        mpz_add(temp, temp, half);        // temp = -to_round + multiple/2
        mpz_tdiv_q(temp, temp, multiple); // temp = (-to_round + multiple/2) / multiple
        mpz_mul(temp, temp, multiple);    // temp = temp * multiple
        mpz_neg(result, temp);            // result = -temp
    }

    mpz_clear(half);
    mpz_clear(temp);
}

void round_encoded_pol_to_nearest_multiple(encoded_polynomial_t *pol, mpz_t multiple)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        mpz_round_to_nearest_multiple(pol->coeffs[i], pol->coeffs[i], multiple);
    }
}

void B_copy_cipher(ciphered_t *c_to_copy, encoded_polynomial_t *M)
{
    for (size_t i = 0; i < c_to_copy->B->size; i++)
    {
        mpz_set(M->coeffs[i], c_to_copy->B->coeffs[i]);
    }
    M->size = c_to_copy->B->size;
}

encoded_polynomial_t *decrypt(ciphered_t *cipher, polynomial_t *secret_key, mpz_t modulo, mpz_t scaling_factor)
{
    encoded_polynomial_t *scaled_M = encoded_pol_init(cipher->B->size);
    if (scaled_M == NULL)
    {
        return NULL;
    }
    B_copy_cipher(cipher, scaled_M);
    polynomial_t *prod = polynomial_mult_modulo(cipher->A, secret_key, modulo); // A * S
    if (prod == NULL)
    {
        return NULL;
    }

    // Removing the secret mask from the ciphered text --> We recovered M_scaled + E
    encoded_pol_add_polynomial_modulo(scaled_M, prod, modulo);

    // fprintf(stdout, "Polynome avec erreur decryption :\n ");
    // encoded_polynomial_print(scaled_M);

    // If E is smaller than scaling_factor / 2, rounding to the nearest multiple of scaling_factor should give us M_scaled
    round_encoded_pol_to_nearest_multiple(scaled_M, scaling_factor);

    encoded_pol_scalar_div_exact(scaled_M, scaling_factor);
    polynomial_free(prod);

    return scaled_M;
}

void encrypt_in_place(ciphered_t *c, encoded_polynomial_t *scaled_M, polynomial_t *secret_key, mpz_t modulo, mpz_t scaling_factor, gmp_randstate_t state)
{
    encoded_pol_scalar_mult(scaled_M, scaling_factor);
    polynomial_t *A = A_generation(scaled_M->size, state, modulo); // MAsk generation
    if (A == NULL)
    {
        return;
    }
    cipher_copy_A(c, A);
    polynomial_t *E = E_generation(scaled_M->size); // Error generation
    if (E == NULL)
    {
        return;
    }
    // fprintf(stdout, "E : \n");
    // polynomial_print(E);

    polynomial_t *prod = polynomial_mult_modulo(A, secret_key, modulo); // A * S
    if (prod == NULL)
    {
        return;
    }

    // Adding error in M, ATTENTION error E should not wrap modulo and every |coefficient| should be less than scaling_factor / 2
    encoded_pol_add_polynomial_modulo(scaled_M, E, modulo);

    // fprintf(stdout, "Polynome avec erreur encryption :\n ");
    // encoded_polynomial_print(scaled_M);

    // Adding secret mask, it can wrap modulo since we sub it at decryption
    mpz_t neg;
    mpz_init_set_si(neg, -1);
    polynomial_scalar_mult(prod, neg);

    encoded_pol_add_polynomial_modulo(scaled_M, prod, modulo);

    cipher_copy_B(c, scaled_M);
    polynomial_free(A);
    polynomial_free(E);
    polynomial_free(prod);
    mpz_clear(neg);
}

void decrypt_in_place(encoded_polynomial_t *decrypted, ciphered_t *cipher, polynomial_t *secret_key, mpz_t modulo, mpz_t scaling_factor)
{
    B_copy_cipher(cipher, decrypted);
    polynomial_t *prod = polynomial_mult_modulo(cipher->A, secret_key, modulo); // A * S
    if (prod == NULL)
    {
        return;
    }

    // Removing the secret mask from the ciphered text --> We recovered M_scaled + E
    encoded_pol_add_polynomial_modulo(decrypted, prod, modulo);

    // fprintf(stdout, "Polynome avec erreur decryption :\n ");
    // encoded_polynomial_print(decrypted);

    // If E is smaller than scaling_factor / 2, rounding to the nearest multiple of scaling_factor should give us M_scaled
    round_encoded_pol_to_nearest_multiple(decrypted, scaling_factor);

    encoded_pol_scalar_div_exact(decrypted, scaling_factor);
    polynomial_free(prod);


}