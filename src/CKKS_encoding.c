#include "CKKS_encoding.h"

int64_t j_in_vandermonde(uint16_t h, size_t size)
{
    int64_t result = 1;
    int64_t mod = 2 * size; // 2 * 1568 = 3136
    for (int64_t i = 0; i < h; i++)
    {
        result = (result * 5) % mod; // modulo à chaque étape pour éviter overflow peut être améliorer
    }
    return (int64_t)result;
}

complex_matrix_t *sigma_basis_tilde_init(size_t size)
{
    complex_matrix_t *basis_matrix = complex_matrix_init(size);
    if (basis_matrix == NULL)
    {
        return NULL;
    }
    float complex primitive_root_of_unity = cexpf((I * M_PI) / size);
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size / 2; j++)
        {
            // fprintf(stdout, "%d : %zu ", i, i * j_in_vandermonde((size / 2) - (j + 1)));
            complex_matrix_set(basis_matrix, i, j, cpowf(primitive_root_of_unity, i * j_in_vandermonde(((size / 2) - (j + 1)), size) % (2 * size)));
        }
        for (size_t j = size / 2; j < size; j++)
        {
            complex_matrix_set(basis_matrix, i, j, cpowf(conjf(primitive_root_of_unity), i * j_in_vandermonde(((size / 2) - (j - (size / 2) + 1)), size) % (2 * size)));
        }
    }
    return basis_matrix;
}

complex_matrix_t *anti_identity_init(size_t size)
{
    complex_matrix_t *anti_I_n = complex_matrix_init(size);
    if (anti_I_n == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (i + j == size - 1)
            {
                complex_matrix_set(anti_I_n, i, j, 1);
            }
            else
            {
                complex_matrix_set(anti_I_n, i, j, 0);
            }
        }
        fprintf(stdout, "\n ");
    }
    return anti_I_n;
}

// Poorly optimized
complex_matrix_t *mult_sigma_basis_anti_identity(size_t size)
{
    complex_matrix_t *sigma_basis = sigma_basis_tilde_init(size);
    if (sigma_basis == NULL)
    {
        return NULL;
    }
    complex_matrix_t *anti_identity = anti_identity_init(size);
    if (anti_identity == NULL)
    {
        return NULL;
    }
    complex_matrix_t *mult = complex_matrix_init(size);

    if (mult == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            for (size_t k = 0; k < size; k++)
            {
                complex_matrix_set(mult, i, j, complex_matrix_get(mult, i, j) + complex_matrix_get(sigma_basis, i, k) * complex_matrix_get(anti_identity, k, j));
            }
        }
    }
    complex_matrix_free(sigma_basis);
    complex_matrix_free(anti_identity);
    return mult;
}

encoded_polynomial_t *complex_vector_encode(complex_vector_t *vector, complex_matrix_t *precomputed_sigma_basis, mpz_t scaling_factor)
{
    encoded_polynomial_t *pol = encoded_pol_init(vector->size);
    if (pol == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < vector->size; i++)
    {
        float temp = 0;
        for (size_t j = 0; j < vector->size; j++)
        {
            temp += complex_matrix_get(precomputed_sigma_basis, i, j) * vector->vector[j];
        }
        temp /= vector->size;
        temp *= mpz_get_ui(scaling_factor); // Precision factor should be less than 2^64 !
        mpz_set_d(pol->coeffs[i], roundf(temp));
    }
    return pol;
}

complex_matrix_t *sigma_basis_tilde_etoile_init(size_t size)
{
    complex_matrix_t *basis_matrix_etoile = complex_matrix_init(size);
    if (basis_matrix_etoile == NULL)
    {
        return NULL;
    }
    float complex primitive_root_of_unity = cexpf((I * M_PI) / (size));
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size / 2; j++)
        {
            complex_matrix_set(basis_matrix_etoile, j, i, cpowf(primitive_root_of_unity, (i * j_in_vandermonde(j, size)) % (2 * size)));
        }
        for (size_t j = size / 2; j < size; j++)
        {
            complex_matrix_set(basis_matrix_etoile, j, i, cpowf(conjf(primitive_root_of_unity), i * j_in_vandermonde((j - (size / 2)) % (2 * size), size)));
        }
    }
    return basis_matrix_etoile;
}

complex_vector_t *recover_vector(encoded_polynomial_t *encoded_pol, complex_matrix_t *sigma_basis_etoile, mpz_t scaling_factor)
{
    complex_vector_t *recovered = complex_vector_init(encoded_pol->size);
    if (recovered == NULL)
    {
        return NULL;
    }
    complex_vector_t *temp = complex_vector_init(encoded_pol->size);
    if (temp == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < encoded_pol->size; i++)
    {
        temp->vector[i] = (float complex)mpz_get_si(encoded_pol->coeffs[i]) / mpz_get_ui(scaling_factor); // i.e plain text size must be in [-2^63, 2^63]
    }
    for (size_t i = 0; i < encoded_pol->size; i++)
    {
        for (size_t j = 0; j < encoded_pol->size; j++)
        {
            recovered->vector[i] += complex_matrix_get(sigma_basis_etoile, i, j) * temp->vector[j];
        }
    }
    complex_vector_free(temp);
    return recovered;
}

void float_encode(const float a, complex_vector_t *v, encoded_polynomial_t *encoded, complex_matrix_t *precomputed_sigma_basis, const mpz_t scaling_factor)
{
    float_to_complex_vector(a, v);
    for (size_t i = 0; i < v->size; i++)
    {
        float temp = 0;
        for (size_t j = 0; j < v->size; j++)
        {
            temp += complex_matrix_get(precomputed_sigma_basis, i, j) * v->vector[j];
        }
        temp /= v->size;
        temp *= mpz_get_ui(scaling_factor); // Precision factor should be less than 2^64 !
        mpz_set_d(encoded->coeffs[i], roundf(temp));
    }
}

float float_decode(encoded_polynomial_t *encoded_pol, complex_matrix_t *sigma_basis_etoile, mpz_t scaling_factor)
{
    complex_vector_t *recovered = recover_vector(encoded_pol, sigma_basis_etoile, scaling_factor); // error handling
    float a = complex_vector_to_float(recovered);
    complex_vector_free(recovered);
    return a;
}