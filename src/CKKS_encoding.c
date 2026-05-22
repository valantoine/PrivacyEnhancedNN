#include "CKKS_encoding.h"

uint64_t j_in_vandermonde(uint16_t h, size_t size)
{
    uint64_t result = 1;
    uint64_t mod = 2 * size; // 2 * 1568 = 3136
    for (uint64_t i = 0; i < h; i++)
    {
        result = (result * 5) % mod; // modulo à chaque étape pour éviter overflow peut être améliorer
    }
    return (uint64_t)result;
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

encoded_polynomial_t *complex_vector_encode(complex_vector_t *vector, complex_matrix_t *precomputed_sigma_basis, uint64_t scaling_factor)
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
            temp += complex_matrix_get(precomputed_sigma_basis, i, j) * vector->vector[j]; // Should not overflow coeff modulus chosen for polynomial by definition
        }
        temp /= vector->size;
        temp *= scaling_factor;
        pol->coeffs[i] = roundf(temp);
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

complex_vector_t *recover_vector(encoded_polynomial_t *encoded_pol, complex_matrix_t *sigma_basis_etoile, uint64_t scaling_factor)
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
        temp->vector[i] = (float complex)encoded_pol->coeffs[i] / scaling_factor;
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

void test_encode_decode()
{
    printf("TEST ENCODAGE/DECODAGE \n");
    // Encode
    size_t size = 4;
    uint64_t scaling_factor = 1024;
    complex_vector_t *complex_vector = complex_vector_init(size);
    complex_vector->vector[0] = 1.1 + I * 4.3;
    complex_vector->vector[1] = 3.5 - I * 1.4;
    complex_vector->vector[2] = conjf(1.1 + I * 4.3);
    complex_vector->vector[3] = conjf(3.5 - I * 1.4);
    complex_vector_print(complex_vector);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol = complex_vector_encode(complex_vector, mult_sigma_IN, scaling_factor);
    polynomial_print(encoded_pol);

    // Decode
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector = recover_vector(encoded_pol, basis_matrix_etoile, scaling_factor);
    complex_vector_print(recovered_complex_vector);

    complex_vector_free(complex_vector);
    complex_vector_free(recovered_complex_vector);
    complex_matrix_free(basis_matrix_etoile);
    complex_matrix_free(mult_sigma_IN);
    encoded_pol_free(encoded_pol);
}

void test_add_encode_decode()
{
    printf("TEST ADDITION \n");
    size_t size = 4;
    uint64_t scaling_factor = 1024;
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 1.1 + I * 4.3;
    complex_vector1->vector[1] = -3.5 - I * 1.4;
    complex_vector1->vector[2] = conjf(1.1 + I * 4.3);
    complex_vector1->vector[3] = conjf(-3.5 - I * 1.4);
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = 0.9 + I * 0.7;
    complex_vector2->vector[1] = 1.5 - I * 2.6;
    complex_vector2->vector[2] = conjf(0.9 + I * 0.7);
    complex_vector2->vector[3] = conjf(1.5 - I * 2.6);
    complex_vector_print(complex_vector2);

    complex_vector_t *complex_vector3 = complex_vector_init(size);
    complex_vector3->vector[0] = 800 + I * 150;
    complex_vector3->vector[1] = -400 - I * 300;
    complex_vector3->vector[2] = conjf(800 + I * 150);
    complex_vector3->vector[3] = conjf(-400 - I * 300);
    complex_vector_print(complex_vector3);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, scaling_factor);
    polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, scaling_factor);
    polynomial_print(encoded_pol2);
    encoded_polynomial_t *encoded_pol3 = complex_vector_encode(complex_vector3, mult_sigma_IN, scaling_factor);
    polynomial_print(encoded_pol3);
    pol_add(encoded_pol1, encoded_pol2);
    // Sum 1 et 2
    polynomial_print(encoded_pol1);

    // sum 1 + 2 + 3
    pol_add(encoded_pol1, encoded_pol3);
    // Decode

    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector1 = recover_vector(encoded_pol1, basis_matrix_etoile, scaling_factor);
    complex_vector_print(recovered_complex_vector1);

    complex_vector_free(complex_vector1);
    complex_vector_free(complex_vector2);
    complex_vector_free(complex_vector3);
    complex_vector_free(recovered_complex_vector1);
    complex_matrix_free(basis_matrix_etoile);
    complex_matrix_free(mult_sigma_IN);
    encoded_pol_free(encoded_pol1);
    encoded_pol_free(encoded_pol2);
    encoded_pol_free(encoded_pol3);
}

void test_mult_encode_decode()
{
    printf("TEST MULTIPLICATION\n");
    size_t size = 4;
    uint64_t scaling_factor = 100000000; 
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 2 + I * 3;
    complex_vector1->vector[1] = 5 - I * 7;
    complex_vector1->vector[2] = conjf(2 + I * 3);
    complex_vector1->vector[3] = conjf(5 - I * 7);
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = 3 + I * 3;
    complex_vector2->vector[1] = 4 - I * 3;
    complex_vector2->vector[2] = conjf(3 + I * 3);
    complex_vector2->vector[3] = conjf(4 - I * 3);
    complex_vector_print(complex_vector2);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, scaling_factor);
    polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, scaling_factor);
    polynomial_print(encoded_pol2);

    encoded_polynomial_t *mult = pol_mult(encoded_pol1, encoded_pol2, scaling_factor);
    // Sum 1 et 2
    polynomial_print(mult);

    // Decode
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector1 = recover_vector(mult, basis_matrix_etoile, scaling_factor);
    complex_vector_print(recovered_complex_vector1);

    complex_vector_free(complex_vector1);
    complex_vector_free(complex_vector2);
    complex_vector_free(recovered_complex_vector1);
    complex_matrix_free(basis_matrix_etoile);
    complex_matrix_free(mult_sigma_IN);
    encoded_pol_free(encoded_pol1);
    encoded_pol_free(encoded_pol2);
    encoded_pol_free(mult);
}