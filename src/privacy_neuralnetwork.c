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
        mpz_set(mat->encoded_cell[i * mat->col_size + j].coeffs[k], pol->coeffs[k]);
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
        ciphered_t *cell = encrypt(&ec->encoded_cell[i], secret_key, modulo_full, scaling_factor, state);
        ciphered_matrix_set(c, i, 0, cell);
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
    for (uint8_t i = 0; i < NB_CLASSES; i++)
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

encoded_matrix_t *encoded_W1(const nn_parameters_t *parameters, mpz_t precision_factor)
{
    encoded_matrix_t *ec = encoded_matrix_init(NB_NEURONS, IMAGE_SIZE);
    if (ec == NULL)
    {
        return NULL;
    }
    complex_matrix_t *precomputed_basis_matrix = mult_sigma_basis_anti_identity(POL_DEGREE); // error handling !
    complex_vector_t *temp = complex_vector_init(POL_DEGREE);
    for (int i = 0; i < NB_NEURONS; i++)
    {
        for (int j = 0; j < IMAGE_SIZE; j++)
        {
            encoded_polynomial_t *cell = encoded_pol_init(POL_DEGREE);
            float_encode(parameters->W1[i][j], temp, cell, precomputed_basis_matrix, precision_factor);
            encoded_matrix_set(ec, i, j, cell);
            encoded_pol_free(cell);
        }
    }
    complex_matrix_free(precomputed_basis_matrix);
    complex_vector_free(temp);
    return ec;
}

encoded_matrix_t *encoded_b1(const nn_parameters_t *parameters, mpz_t precision_factor)
{
    encoded_matrix_t *ec = encoded_matrix_init(NB_NEURONS, 1);
    if (ec == NULL)
    {
        return NULL;
    }
    complex_matrix_t *precomputed_basis_matrix = mult_sigma_basis_anti_identity(POL_DEGREE); // error handling !
    complex_vector_t *temp = complex_vector_init(POL_DEGREE);
    for (int i = 0; i < NB_NEURONS; i++)
    {

        encoded_polynomial_t *cell = encoded_pol_init(POL_DEGREE);
        float_encode(parameters->b1[i], temp, cell, precomputed_basis_matrix, precision_factor);
        encoded_matrix_set(ec, i, 0, cell);
        encoded_pol_free(cell);
    }
    complex_matrix_free(precomputed_basis_matrix);
    complex_vector_free(temp);
    return ec;
}

encoded_matrix_t *encoded_W2(const nn_parameters_t *parameters, mpz_t precision_factor)
{
    encoded_matrix_t *ec = encoded_matrix_init(NB_CLASSES, NB_NEURONS);
    if (ec == NULL)
    {
        return NULL;
    }
    complex_matrix_t *precomputed_basis_matrix = mult_sigma_basis_anti_identity(POL_DEGREE); // error handling !
    complex_vector_t *temp = complex_vector_init(POL_DEGREE);
    for (int i = 0; i < NB_CLASSES; i++)
    {
        for (int j = 0; j < NB_NEURONS; j++)
        {
            encoded_polynomial_t *cell = encoded_pol_init(POL_DEGREE);
            float_encode(parameters->W2[i][j], temp, cell, precomputed_basis_matrix, precision_factor);
            encoded_matrix_set(ec, i, j, cell);
            encoded_pol_free(cell);
        }
    }
    complex_matrix_free(precomputed_basis_matrix);
    complex_vector_free(temp);
    return ec;
}

encoded_matrix_t *encoded_b2(const nn_parameters_t *parameters, mpz_t precision_factor)
{
    encoded_matrix_t *ec = encoded_matrix_init(NB_CLASSES, 1);
    if (ec == NULL)
    {
        return NULL;
    }
    complex_matrix_t *precomputed_basis_matrix = mult_sigma_basis_anti_identity(POL_DEGREE); // error handling !
    complex_vector_t *temp = complex_vector_init(POL_DEGREE);
    for (int i = 0; i < NB_CLASSES; i++)
    {

        encoded_polynomial_t *cell = encoded_pol_init(POL_DEGREE);
        float_encode(parameters->b2[i], temp, cell, precomputed_basis_matrix, precision_factor);
        encoded_matrix_set(ec, i, 0, cell);
        encoded_pol_free(cell);
    }
    complex_matrix_free(precomputed_basis_matrix);
    complex_vector_free(temp);
    return ec;
}

void decrease_mult_level(mpz_t current_modulo, mpz_t scaling_factor)
{
    mpz_divexact(current_modulo, current_modulo, scaling_factor);
}

ciphered_t *precompute_evk_g(polynomial_t *secret_key, mpz_t modulo_full, mpz_t g, gmp_randstate_t state, mpz_t scaling_factor)
{
    mpz_t mod_copy;
    mpz_init_set(mod_copy, modulo_full);
    mpz_divexact(mod_copy, mod_copy, scaling_factor); // We will need evk_g only on one multiplicative level in our network which is after one plain multiplication so one level down from start.

    ciphered_t *precomputed_evk_g = evk_g(secret_key, mod_copy, g, state);

    mpz_clear(mod_copy);
    return precomputed_evk_g;
}

void square_activation(ciphered_matrix_t *c, ciphered_t *precomputed_evk_g, mpz_t current_modulo, mpz_t g, mpz_t scaling_factor)
{
    for (size_t i = 0; i < c->row_size; i++)
    {
        for (size_t j = 0; j < c->col_size; j++)
        {
            ciphered_t *cell = ciphered_matrix_get(c, i, j);
            ciphered_t *cell_squared = ckks_mult_cipher_cipher(cell, cell, current_modulo, precomputed_evk_g, g, scaling_factor);
            ciphered_matrix_set(c, i, j, cell_squared);
            cipher_free(cell_squared);
        }
    }
}

ciphered_matrix_t *ciphered_feed_forward(ciphered_matrix_t *input, const nn_parameters_t *parameters, ciphered_t *precomputed_evk_g, mpz_t precomputed_g, mpz_t modulo_full, mpz_t scaling_factor, mpz_t precision_factor)
{
    encoded_matrix_t *W1_ec = encoded_W1(parameters, precision_factor);
    encoded_matrix_t *b1_ec = encoded_b1(parameters, precision_factor);
    encoded_matrix_t *W2_ec = encoded_W2(parameters, precision_factor);
    encoded_matrix_t *b2_ec = encoded_b2(parameters, precision_factor);
    mpz_t current_modulo;
    mpz_init_set(current_modulo, modulo_full);
    mpz_t current_modulo_down; // temp variable used to do the sum on cells after multiplications since every cipher operations must be down under the same modulo
    mpz_init(current_modulo_down);
    mpz_divexact(current_modulo_down, current_modulo, scaling_factor);

    // First step Z1 (here A1) = W1 * X:
    ciphered_matrix_t *A1 = ciphered_matrix_init(NB_NEURONS, 1); // NULL matrix for the moment
    for (int j = 0; j < NB_NEURONS; j++)
    {
        for (int k = 0; k < IMAGE_SIZE; k++)
        {
            ciphered_t *cipher_cell_X = ciphered_matrix_get(input, k, 0);                                                    // Image[k]
            encoded_polynomial_t *encoded_cell_W1 = encoded_matrix_get(W1_ec, j, k);                                         // W1[j][k]
            ciphered_t *W1_times_X = ckks_mult_cipher_plain(cipher_cell_X, encoded_cell_W1, current_modulo, scaling_factor); // Image[k] * W1[j][k]
            ciphered_t *A1_cell = ciphered_matrix_get(A1, j, 0);                                                             // A1[j]
            ckks_add_cipher_cipher(A1_cell, A1_cell, W1_times_X, current_modulo_down);                                       // A1[j] = A1[j] + Image[k] * W1[j][k]
            cipher_free(W1_times_X);
        }
    }
    mpz_divexact(current_modulo, current_modulo, scaling_factor); // We have done one mult so we decrease the current modulo
    mpz_divexact(current_modulo_down, current_modulo_down, scaling_factor);

    // Second step : Z1 = Z1 + b1

    for (int i = 0; i < NB_NEURONS; i++)
    {
        ciphered_t *cipher_cell_A1 = ciphered_matrix_get(A1, i, 0);                                             // A1[j]
        encoded_polynomial_t *encoded_cell_b1 = encoded_matrix_get(b1_ec, i, 0);                                // b1[j]
        ckks_add_cipher_plain(cipher_cell_A1, cipher_cell_A1, encoded_cell_b1, current_modulo, scaling_factor); // A1[j] = A1[j] + b1[j]
    }

    // Third step : A1 = Z1^2 i.e activation function

    square_activation(A1, precomputed_evk_g, current_modulo, precomputed_g, scaling_factor);
    mpz_divexact(current_modulo, current_modulo, scaling_factor); // We have done one mult so we decrease the current modulo i.e 2 multiplicative level down from start
    mpz_divexact(current_modulo_down, current_modulo_down, scaling_factor);

    // Fourth step : Z2 = W2 * A1

    ciphered_matrix_t *Z2 = ciphered_matrix_init(NB_CLASSES, 1); // NULL matrix for the moment
    for (int j = 0; j < NB_CLASSES; j++)
    {
        for (int k = 0; k < NB_NEURONS; k++)
        {
            ciphered_t *cipher_cell_A1 = ciphered_matrix_get(A1, k, 0);                                                        // A1[k]
            encoded_polynomial_t *encoded_cell_W2 = encoded_matrix_get(W2_ec, j, k);                                           // W2[j][k]
            ciphered_t *W2_times_A1 = ckks_mult_cipher_plain(cipher_cell_A1, encoded_cell_W2, current_modulo, scaling_factor); // A1[k] * W2[j][k]
            ciphered_t *Z2_cell = ciphered_matrix_get(Z2, j, 0);                                                               // Z2[j]
            ckks_add_cipher_cipher(Z2_cell, Z2_cell, W2_times_A1, current_modulo_down);                                        // Z2[j] = Z2[j] + A1[k] * W2[j][k]
            cipher_free(W2_times_A1);
        }
    }

    mpz_divexact(current_modulo, current_modulo, scaling_factor); // We have done one mult so we decrease the current modulo
    // We will not do any more mult so no need for current_modulo_down. We should have reached the q_0 chosen at the beginning

    // Fifth and last step : Z2 = Z2 + b2
    for (int i = 0; i < NB_CLASSES; i++)
    {
        ciphered_t *cipher_cell_Z2 = ciphered_matrix_get(Z2, i, 0);                                             // Z2[j]
        encoded_polynomial_t *encoded_cell_b2 = encoded_matrix_get(b2_ec, i, 0);                                // b2[j]
        ckks_add_cipher_plain(cipher_cell_Z2, cipher_cell_Z2, encoded_cell_b2, current_modulo, scaling_factor); // Z2[j] = Z2[j] + b2[j]
    }

    // To simplify we don't do another activation function like sotftmax. (however it could be useful for back propagation later)

    encoded_matrix_free(W1_ec);
    encoded_matrix_free(b1_ec);
    encoded_matrix_free(W2_ec);
    encoded_matrix_free(b2_ec);
    mpz_clear(current_modulo);
    mpz_clear(current_modulo_down);
    ciphered_matrix_free(A1);

    return Z2;
}

void encryption_parameters_init(mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_0, mpz_t modulo_full)
{
    mpz_init_set_ui(precision_factor, PRECISION_VALUE); // for encoding

    mpz_init(scaling_factor);
    mpz_ui_pow_ui(scaling_factor, 2, SCALING_FACTOR_BIT_SIZE); // 2^(SCALING_FACTOR_BIT_SIZE)

    mpz_init(modulo_0);
    mpz_ui_pow_ui(modulo_0, 2, MODULO_0_BIT_SIZE); // 2^(MODULO_0_BIT_SIZE)

    mpz_init(modulo_full);
    mpz_ui_pow_ui(modulo_full, 2, MODULO_0_BIT_SIZE + (SCALING_FACTOR_BIT_SIZE * MULTIPLICATIVE_LEVEL_MAX)); // modulo_full = modulo_0 * scaling_factor^L, where L is the maximum multiplicative level, this number is huge.

    
}

void encryption_parameters_clear(mpz_t precision_factor, mpz_t scaling_factor, mpz_t modulo_0, mpz_t modulo_full)
{
    mpz_clear(precision_factor);
    mpz_clear(scaling_factor);
    mpz_clear(modulo_0);
    mpz_clear(modulo_full);
}

uint8_t predict_image_private(dataset_t *test_dataset, const nn_parameters_t *parameters, polynomial_t *secret_key, int verbose, gmp_randstate_t state)
{

    // Selecting one image
    image_t *original_image = test_dataset->images; // Keep address of the images array
    size_t original_size = test_dataset->size;
    int random_image_index = rand() % TEST_SIZE; // Pick a random number
    test_dataset->images = original_image + random_image_index;
    test_dataset->size = 1;
    fprintf(stdout, "Image picked : \n");
    image_print(&test_dataset->images[0], stdout);

    // parameters init
    mpz_t precision_factor;
    mpz_t scaling_factor;
    mpz_t modulo_0;
    mpz_t modulo_full;
    encryption_parameters_init(precision_factor, scaling_factor, modulo_0, modulo_full);

    // Precomputing g and evk_g
    mpz_t g;
    mpz_init(g);
    big_power_of_2_below(g, modulo_full);
    ciphered_t *precomputed_evk_g = precompute_evk_g(secret_key, modulo_full, g, state, scaling_factor);

    // Encrypting
    ciphered_matrix_t *encrypted_image = encrypt_image(test_dataset->images[0], secret_key, verbose, precision_factor, scaling_factor, modulo_full, state);

    // Feed forward
    ciphered_matrix_t *ciphered_prediction = ciphered_feed_forward(encrypted_image, parameters, precomputed_evk_g, g, modulo_full, scaling_factor, precision_factor);

    //Decrypting prediction
    mpz_t updated_precision_factor;
    mpz_init_set(updated_precision_factor, precision_factor);
    mpz_pow_ui(precision_factor, precision_factor, MULTIPLICATIVE_LEVEL_MAX); //Each ciphered has been multiplied 3 times i.e the precision factor during decoding is precision_factor^3 
    uint8_t pred = decrypt_prediction(ciphered_prediction, secret_key, verbose, updated_precision_factor, scaling_factor, modulo_0);

    encryption_parameters_clear(precision_factor, scaling_factor, modulo_0, modulo_full);
    cipher_free(precomputed_evk_g);
    ciphered_matrix_free(encrypted_image);
    ciphered_matrix_free(ciphered_prediction);
    mpz_clear(updated_precision_factor);

    test_dataset->images = original_image;
    test_dataset->size = original_size;
    return pred;
}
