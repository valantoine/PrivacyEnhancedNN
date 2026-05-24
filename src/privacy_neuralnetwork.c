#include "privacy_neuralnetwork.h"

encoded_matrix_t *encoded_matrix_init(size_t rows, size_t cols)
{
    encoded_matrix_t *mat = malloc(sizeof(encoded_matrix_t));
    if (mat == NULL)
    {
        fprintf(stderr, "encoded_matrix_init : Failed to allocate matrix\n");
        return NULL;
    }

    mat->row_size = rows;
    mat->col_size = cols;

    mat->encoded_cell = malloc(sizeof(encoded_polynomial_t) * rows * cols);
    if (mat->encoded_cell == NULL)
    {
        fprintf(stderr, "encoded_matrix_init : Failed to allocate cells\n");
        free(mat);
        return NULL;
    }

    for (size_t i = 0; i < rows * cols; i++)
    {
        encoded_pol_init_in_place(&mat->encoded_cell[i], POL_DEGREE);
    }

    return mat;
}

void encoded_matrix_free(encoded_matrix_t *mat)
{
    for (size_t i = 0; i < mat->row_size * mat->col_size; i++)
    {
        for (size_t j = 0; j < mat->encoded_cell[i].size; j++)
        {
            mpz_clear(mat->encoded_cell[i].coeffs[j]);
        }
        free(mat->encoded_cell[i].coeffs);
    }
    free(mat->encoded_cell);
    free(mat);
}

encoded_polynomial_t *encoded_matrix_get(encoded_matrix_t *mat, size_t i, size_t j)
{
    if (i >= mat->row_size || j >= mat->col_size)
    {
        fprintf(stderr, "encoded_matrix_get : wrong size\n");
        return NULL;
    }
    return &mat->encoded_cell[i * mat->col_size + j];
}

void encoded_matrix_set(encoded_matrix_t *mat, size_t i, size_t j, encoded_polynomial_t *pol)
{
    if (i >= mat->row_size || j >= mat->col_size)
    {
        fprintf(stderr, "encoded_matrix_set : Index out of bounds\n");
    }

    encoded_polynomial_t *cell = &mat->encoded_cell[i * mat->col_size + j];

    if (cell->size != pol->size)
    {
        fprintf(stderr, "encoded_matrix_set : Polynomial size mismatch\n");
    }

    for (size_t k = 0; k < pol->size; k++)
    {
        mpz_set(mat->encoded_cell->coeffs[i * mat->col_size + j], pol->coeffs[k]);
    }
}

ciphered_matrix_t *ciphered_matrix_init(size_t rows, size_t cols)
{
    ciphered_matrix_t *mat = malloc(sizeof(ciphered_matrix_t));
    if (mat == NULL)
    {
        fprintf(stderr, "ciphered_matrix_init : Failed to allocate matrix\n");
        return NULL;
    }

    mat->row_size = rows;
    mat->col_size = cols;

    mat->ciphered_matrix = malloc(sizeof(ciphered_t) * rows * cols);
    if (mat->ciphered_matrix == NULL)
    {
        fprintf(stderr, "ciphered_matrix_init : Failed to allocate cells\n");
        free(mat);
        return NULL;
    }

    for (size_t i = 0; i < rows * cols; i++)
    {
        cipher_init_in_place(&mat->ciphered_matrix[i], POL_DEGREE);
    }

    return mat;
}

void ciphered_matrix_free(ciphered_matrix_t *mat)
{
    for (size_t i = 0; i < mat->row_size * mat->col_size; i++)
    {
        polynomial_free(mat->ciphered_matrix[i].A);
        encoded_pol_free(mat->ciphered_matrix[i].B);
    }
    free(mat->ciphered_matrix);
    free(mat);
}

ciphered_t *ciphered_matrix_get(ciphered_matrix_t *mat, size_t i, size_t j)
{
    if (i >= mat->row_size || j >= mat->col_size)
    {
        fprintf(stderr, "ciphered_matrix_get : Index out of bounds\n");
        return NULL;
    }
    return &mat->ciphered_matrix[i * mat->col_size + j];
}

void ciphered_matrix_set(ciphered_matrix_t *mat, size_t i, size_t j, ciphered_t *c)
{
    if (i >= mat->row_size || j >= mat->col_size)
    {
        fprintf(stderr, "ciphered_matrix_set : Index out of bounds\n");
    }

    // Copy A
    for (size_t k = 0; k < c->B->size; k++)
    {
        mpz_set(mat->ciphered_matrix[i].A->coeffs[k], c->A->coeffs[k]);
    }

    // Copy B
    for (size_t k = 0; k < c->B->size; k++)
    {
        mpz_set(mat->ciphered_matrix[i].B->coeffs[k], c->B->coeffs[k]);
    }
}

ciphered_matrix_t *encrypt_image(image_t image, polynomial_t *secret_key, int verbose, mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_full, gmp_randstate_t state)
{
    ciphered_matrix_t *c = ciphered_matrix_init(IMAGE_SIZE, 1); // Vector of size 728
    if (c == NULL)
    {
        return NULL;
    }
    encoded_matrix_t *ec = encoded_matrix_init(IMAGE_SIZE, 1);
    if (ec == NULL)
    {
        return NULL;
    }

    // Encoding vector
    complex_matrix_t *precomputed_basis_matrix = mult_sigma_basis_anti_identity(POL_DEGREE); // error handling !
    complex_vector_t *temp = complex_vector_init(POL_DEGREE);                                // is refilled every time in encode
    for (int i = 0; i < IMAGE_SIZE; i++)
    {
        encoded_polynomial_t *cell = encoded_pol_init(POL_DEGREE);
        float_encode(image.image[i], temp, cell, precomputed_basis_matrix, precision_factor);
        encoded_matrix_set(ec, i, 0, cell);
        encoded_pol_free(cell);
    }
    if (verbose == 1)
    {
        printf("Encoded image :\n");
        // TO DO
    }

    // Encrypting vector
    for (int i = 0; i < IMAGE_SIZE; i++)
    {
        ciphered_t *cell = cipher_init(POL_DEGREE);
        encrypt_in_place(cell, &ec->encoded_cell[i], secret_key, modulo_full, scaling_factor, state);
        ciphered_matrix_set(c, 1, 0, cell);
        cipher_free(cell);
    }
    if (verbose == 1)
    {
        printf("Encypted image :\n");
        // TO DO
    }
    encoded_matrix_free(ec);
    complex_matrix_free(precomputed_basis_matrix);
    complex_vector_free(temp);
    return c;
}

uint8_t decrypt_prediction(ciphered_matrix_t *c, polynomial_t *secret_key, int verbose, mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_0)
{
    uint8_t max_cell = 0;
    float max = 0;
    // Decrypting vector
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(POL_DEGREE);
    for (int i = 0; i < NB_CLASSES; i++)
    {
        encoded_polynomial_t *decrypted_cell = encoded_pol_init(POL_DEGREE);
        ciphered_t *ciphered_cell = ciphered_matrix_get(c, i, 0);
        if (verbose == 1)
        {
            printf("Encrypted cell %d : ", i);
        }
        decrypt_in_place(decrypted_cell, ciphered_cell, secret_key, modulo_0, scaling_factor); // Modulo_0 is the minimum modulo level reached and chosen at the beginning
        if (verbose == 1)
        {
            printf("Decrypted cell %d : ", i);
        }
        float recovered_value = float_decode(decrypted_cell, basis_matrix_etoile, precision_factor); // precision factor has been increased with mult before
        if (verbose == 1)
        {
            printf("Decoded cell %i : %f", i, recovered_value);
        }
        if (recovered_value > max)
        {
            max = recovered_value;
            max_cell = i; // prediction
        }
    }
    return max_cell;
}