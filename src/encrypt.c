#include "encrypt.h"

polynomial_t *key_generation(size_t size)
{
    polynomial_t *key = polyonimal_init(size);
    if (key == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        key->coeffs[i] = (rand() % 3) - 1; // Coefficients among {-1,0,1}
    }
    return key;
}

polynomial_t *A_generation(size_t size)
{
    polynomial_t *A = polyonimal_init(size);
    if (A == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        A->coeffs[i] = (rand() % 257) - 128; // A changer plus tard en fonction du modulo Q
    }
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
    polynomial_t *E = polyonimal_init(size);
    if (E == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        E->coeffs[i] = (int64_t)round(gauss()); // A changer plus tard en fonction du modulo Q
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
    c->A = polyonimal_init(size);
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
        c->A->coeffs[i] = A_to_copy->coeffs[i];
    }
    c->A->degree = A_to_copy->degree;
}

void cipher_copy_B(ciphered_t *c, encoded_polynomial_t *B_to_copy)
{
    for (size_t i = 0; i < B_to_copy->size; i++)
    {
        c->B->coeffs[i] = B_to_copy->coeffs[i];
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
ciphered_t *encrypt(encoded_polynomial_t *scaled_M, polynomial_t *secret_key, uint64_t modulo)
{

    ciphered_t *c = cipher_init(scaled_M->size);
    if (c == NULL)
    {
        return NULL;
    }
    polynomial_t *A = A_generation(scaled_M->size); // MAsk generation
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
    fprintf(stdout, "E : \n");
    polynomial_print(E);

    polynomial_t *prod = polynomial_mult_modulo(A, secret_key, modulo); // -A * S
    if (prod == NULL)
    {
        return NULL;
    }

    // Adding error in M, ATTENTION error E should not wrap modulo and every |coefficient| should be less than scaling_factor / 2
    encoded_pol_add_polynomial_modulo(scaled_M, E, modulo);

    fprintf(stdout, "Polynome avec erreur encryption :\n ");
    encoded_polynomial_print(scaled_M);

    // Adding secret mask, it can wrap modulo since we sub it at decryption
    polynomial_scalar_mult(prod, -1);

    encoded_pol_add_polynomial_modulo(scaled_M, prod, modulo);

    cipher_copy_B(c, scaled_M);
    polynomial_free(A);
    polynomial_free(E);
    polynomial_free(prod);
    return c;
}

int64_t round_to_nearest_multiple(int64_t to_round, uint64_t multiple)
{
    return (int64_t)round((double)to_round / (double)multiple) * multiple;
}

void round_encoded_pol_to_nearest_multiple(encoded_polynomial_t *pol, uint64_t multiple)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        pol->coeffs[i] = round_to_nearest_multiple(pol->coeffs[i], multiple);
    }
}

void B_copy_cipher(ciphered_t *c_to_copy, encoded_polynomial_t *M)
{
    for (size_t i = 0; i < c_to_copy->B->size; i++)
    {
        M->coeffs[i] = c_to_copy->B->coeffs[i];
    }
    M->size = c_to_copy->B->size;
}

encoded_polynomial_t *decrypt(ciphered_t *cipher, polynomial_t *secret_key, uint64_t modulo, uint64_t scaling_factor)
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

    fprintf(stdout, "Polynome avec erreur decryption :\n ");
    encoded_polynomial_print(scaled_M);

    // If E is smaller than scaling_factor / 2, rounding to the nearest multiple of scaling_factor should give us M_scaled
    round_encoded_pol_to_nearest_multiple(scaled_M, scaling_factor);


    polynomial_free(prod);

    return scaled_M;
}

void test_encrypt_decrypt()
{
    // Encode
    fprintf(stdout, "MAX INT :%" PRId64 " MIN INT :%" PRId64"\n", INT64_MAX, INT64_MIN);
    size_t size = 2;
    int64_t encoding_precision_factor = 1024;
    int64_t scaling_factor = 64;
    complex_vector_t *complex_vector = complex_vector_init(size);
    complex_vector->vector[0] = 10000.5125;
    complex_vector->vector[1] = conjf(10000.5125);
    complex_vector_print(complex_vector);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol = complex_vector_encode(complex_vector, mult_sigma_IN, encoding_precision_factor);
    encoded_polynomial_print(encoded_pol);

    // Encrypt
    polynomial_t *secret_key = key_generation(size);

    encoded_pol_scalar_mult(encoded_pol, scaling_factor);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);
    int64_t modulo = INT64_MAX;
    ciphered_t *c = encrypt(encoded_pol, secret_key, modulo);
    cipher_print(c);

    // Decrypt
    encoded_polynomial_t *scaled_M = decrypt(c, secret_key, modulo, scaling_factor);
    encoded_pol_scalar_div(scaled_M, scaling_factor);
    fprintf(stdout, "Polynome after decryption : \n");
    encoded_polynomial_print(scaled_M);

    // Decode
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector = recover_vector(scaled_M, basis_matrix_etoile, encoding_precision_factor);
    complex_vector_print(recovered_complex_vector);

    complex_vector_free(complex_vector);
    complex_matrix_free(mult_sigma_IN);
    encoded_pol_free(encoded_pol);
    polynomial_free(secret_key);
    cipher_free(c);
    encoded_pol_free(scaled_M);
    complex_matrix_free(basis_matrix_etoile);
    complex_vector_free(recovered_complex_vector);
}
