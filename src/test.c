#include "test.h"
void test_encode_decode()
{
    printf("TEST ENCODAGE/DECODAGE \n");
    // Encode
    mpz_t precision_factor;
    size_t size = 4;
    mpz_init_set_ui(precision_factor, 512);
    complex_vector_t *complex_vector = complex_vector_init(size);
    complex_vector->vector[0] = 1.1 + I * 4.3;
    complex_vector->vector[1] = 3.5 - I * 1.4;
    complex_vector->vector[2] = conjf(1.1 + I * 4.3);
    complex_vector->vector[3] = conjf(3.5 - I * 1.4);
    complex_vector_print(complex_vector);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol = complex_vector_encode(complex_vector, mult_sigma_IN, precision_factor);
    encoded_polynomial_print(encoded_pol);

    // Decode
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector = recover_vector(encoded_pol, basis_matrix_etoile, precision_factor);
    complex_vector_print(recovered_complex_vector);

    complex_vector_free(complex_vector);
    complex_vector_free(recovered_complex_vector);
    complex_matrix_free(basis_matrix_etoile);
    complex_matrix_free(mult_sigma_IN);
    encoded_pol_free(encoded_pol);
    mpz_clear(precision_factor);
}

void test_add_encode_decode()
{
    printf("TEST ADDITION \n");
    size_t size = 4;
    mpz_t precision_factor;
    mpz_init_set_ui(precision_factor, 512);
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
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, precision_factor);
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, precision_factor);
    encoded_polynomial_print(encoded_pol2);
    encoded_polynomial_t *encoded_pol3 = complex_vector_encode(complex_vector3, mult_sigma_IN, precision_factor);
    encoded_polynomial_print(encoded_pol3);
    encoded_pol_add(encoded_pol1, encoded_pol2);
    // Sum 1 et 2
    encoded_polynomial_print(encoded_pol1);

    // sum 1 + 2 + 3
    encoded_pol_add(encoded_pol1, encoded_pol3);
    // Decode

    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector1 = recover_vector(encoded_pol1, basis_matrix_etoile, precision_factor);
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
    mpz_clear(precision_factor);
}

void test_mult_encode_decode()
{
    printf("TEST MULTIPLICATION\n");
    size_t size = 4;
    mpz_t precision_factor;
    mpz_init_set_ui(precision_factor, 512);
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 2.5;
    complex_vector1->vector[1] = 1.2;
    complex_vector1->vector[2] = conjf(2.5);
    complex_vector1->vector[3] = conjf(1.2);
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = 4;
    complex_vector2->vector[1] = 5;
    complex_vector2->vector[2] = conjf(4);
    complex_vector2->vector[3] = conjf(5);
    complex_vector_print(complex_vector2);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, precision_factor);
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, precision_factor);
    encoded_polynomial_print(encoded_pol2);

    encoded_polynomial_t *mult = encoded_pol_mult_rescaled(encoded_pol1, encoded_pol2, precision_factor);
    // Sum 1 et 2
    encoded_polynomial_print(mult);

    // Decode
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector1 = recover_vector(mult, basis_matrix_etoile, precision_factor);
    complex_vector_print(recovered_complex_vector1);

    complex_vector_free(complex_vector1);
    complex_vector_free(complex_vector2);
    complex_vector_free(recovered_complex_vector1);
    complex_matrix_free(basis_matrix_etoile);
    complex_matrix_free(mult_sigma_IN);
    encoded_pol_free(encoded_pol1);
    encoded_pol_free(encoded_pol2);
    encoded_pol_free(mult);
    mpz_clear(precision_factor);
}

void test_encrypt_decrypt()
{
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    // Encode
    fprintf(stdout, "MAX INT :%" PRId64 " MIN INT :%" PRId64 "\n", INT64_MAX, INT64_MIN);
    size_t size = 2;
    mpz_t encoding_precision_factor;
    mpz_init_set_ui(encoding_precision_factor, 512); // precision factor
    mpz_t scaling_factor;
    mpz_init_set_ui(scaling_factor, 64); // delta = 64
    mpz_t modulo;
    mpz_init_set_ui(modulo, INT64_MAX); // q= INT_MAX
    complex_vector_t *complex_vector = complex_vector_init(size);
    complex_vector->vector[0] = 10000.5125;
    complex_vector->vector[1] = conjf(10000.5125);
    complex_vector_print(complex_vector);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol = complex_vector_encode(complex_vector, mult_sigma_IN, encoding_precision_factor);
    encoded_polynomial_print(encoded_pol);

    // Encrypt
    polynomial_t *secret_key = key_generation(size, state);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);

    ciphered_t *c = encrypt(encoded_pol, secret_key, modulo, scaling_factor, state);
    cipher_print(c);

    // Decrypt
    encoded_polynomial_t *scaled_M = decrypt(c, secret_key, modulo, scaling_factor);
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
    mpz_clear(encoding_precision_factor);
    mpz_clear(scaling_factor);
    mpz_clear(modulo);
    gmp_randclear(state);
}

void test_encrypt_add_cipher()
{
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    // encoding
    printf("TEST ENCRYPTED ADDITION \n");
    size_t size = 2; // N = 2
    mpz_t encoding_precision_factor;
    mpz_init_set_ui(encoding_precision_factor, 256); // precision factor
    mpz_t scaling_factor;
    mpz_init_set_ui(scaling_factor, 512); // delta
    mpz_t modulo;
    mpz_init_set_ui(modulo, INT64_MAX); // q= INT_MAX
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 18;
    complex_vector1->vector[1] = conjf(18);
    fprintf(stdout, "First vector : \n");
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = -5;
    complex_vector2->vector[1] = conjf(-5);
    fprintf(stdout, "Second vector : \n");
    complex_vector_print(complex_vector2);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "First encoded vector : \n");
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "Second encoded vector : \n");
    encoded_polynomial_print(encoded_pol2);

    // Encrypting
    polynomial_t *secret_key = key_generation(size, state);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);

    ciphered_t *c1 = encrypt(encoded_pol1, secret_key, modulo, scaling_factor, state);
    ciphered_t *c2 = encrypt(encoded_pol2, secret_key, modulo, scaling_factor, state);
    fprintf(stdout, "First ciphered vector : \n");
    cipher_print(c1);

    fprintf(stdout, "Second ciphered vector : \n");
    cipher_print(c2);

    // Addition :
    ciphered_t *sum = cipher_init(size);
    ckks_add_cipher_cipher(sum, c1, c2, modulo);
    fprintf(stdout, "SUM ");
    cipher_print(sum);

    // Decrypting
    encoded_polynomial_t *scaled_M = decrypt(sum, secret_key, modulo, scaling_factor);
    fprintf(stdout, "Sum after decryption : \n");
    encoded_polynomial_print(scaled_M);

    // Decoding
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector = recover_vector(scaled_M, basis_matrix_etoile, encoding_precision_factor);
    fprintf(stdout, "Recovered vector");
    complex_vector_print(recovered_complex_vector);
    mpz_clear(encoding_precision_factor);
    mpz_clear(scaling_factor);
    mpz_clear(modulo);
    gmp_randclear(state);

    // Free later
}

void test_encrypt_add_plain()
{
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    // encoding
    printf("TEST ENCRYPTED ADDITION \n");
    size_t size = 2; // N = 2
    mpz_t encoding_precision_factor;
    mpz_init_set_ui(encoding_precision_factor, 256); // precision factor
    mpz_t scaling_factor;
    mpz_init_set_ui(scaling_factor, 512); // delta
    mpz_t modulo;
    mpz_init_set_ui(modulo, INT64_MAX); // q= INT_MAX
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 3.5;
    complex_vector1->vector[1] = conjf(3.5);
    fprintf(stdout, "First vector : \n");
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = 10;
    complex_vector2->vector[1] = conjf(10);
    fprintf(stdout, "Second vector : \n");
    complex_vector_print(complex_vector2);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "First encoded vector : \n");
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "Second encoded vector : \n");
    encoded_polynomial_print(encoded_pol2);

    // Encrypting
    polynomial_t *secret_key = key_generation(size, state);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);
    ciphered_t *c1 = encrypt(encoded_pol1, secret_key, modulo, scaling_factor, state);
    fprintf(stdout, "First ciphered vector : \n");
    cipher_print(c1);

    // Addition :
    ciphered_t *sum = cipher_init(size);
    ckks_add_cipher_plain(sum, c1, encoded_pol2, modulo, scaling_factor);
    fprintf(stdout, "SUM ");
    cipher_print(sum);

    // Decrypting
    encoded_polynomial_t *scaled_M = decrypt(sum, secret_key, modulo, scaling_factor);
    fprintf(stdout, "Sum after decryption : \n");
    encoded_polynomial_print(scaled_M);

    // Decoding
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    complex_vector_t *recovered_complex_vector = recover_vector(scaled_M, basis_matrix_etoile, encoding_precision_factor);
    fprintf(stdout, "Recovered vector");
    complex_vector_print(recovered_complex_vector);
    mpz_clear(encoding_precision_factor);
    mpz_clear(scaling_factor);
    mpz_clear(modulo);
    gmp_randclear(state);

    // Free later
}

void test_encrypt_mult_plain()
{
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    // encoding
    printf("TEST ENCRYPTED MULTIPLICATION WITH PLAIN \n");
    size_t size = 2; // N = 2
    mpz_t encoding_precision_factor;
    mpz_init_set_ui(encoding_precision_factor, 512); // precision factor
    mpz_t scaling_factor;
    mpz_init_set_ui(scaling_factor, 1048576); // delta 2^20;
    mpz_t modulo;
    mpz_init_set_ui(modulo, INT64_MAX);
    mpz_mul(modulo, modulo, scaling_factor); // Q = INT_MAX * scaling_factor
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 0.2;
    complex_vector1->vector[1] = conjf(0.2);
    fprintf(stdout, "First vector : \n");
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = 0.5;
    complex_vector2->vector[1] = conjf(0.5);
    fprintf(stdout, "Second vector : \n");
    complex_vector_print(complex_vector2);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "First encoded vector : \n");
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "Second encoded vector : \n");
    encoded_polynomial_print(encoded_pol2);

    // Encrypting
    polynomial_t *secret_key = key_generation(size, state);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);
    ciphered_t *c1 = encrypt(encoded_pol1, secret_key, modulo, scaling_factor, state);
    fprintf(stdout, "First ciphered vector : \n");
    cipher_print(c1);

    // Multiplication
    ciphered_t *mult = ckks_mult_cipher_plain(c1, encoded_pol2, modulo, scaling_factor);
    fprintf(stdout, "MULT :\n");
    cipher_print(mult);

    encoded_polynomial_t *scaled_M = decrypt(mult, secret_key, modulo, scaling_factor);
    fprintf(stdout, "Mult after decryption : \n");
    encoded_polynomial_print(scaled_M);

    // Decoding
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    mpz_mul(encoding_precision_factor, encoding_precision_factor, encoding_precision_factor);
    complex_vector_t *recovered_complex_vector = recover_vector(scaled_M, basis_matrix_etoile, encoding_precision_factor);
    fprintf(stdout, "Recovered vector");
    complex_vector_print(recovered_complex_vector);
    mpz_clear(encoding_precision_factor);
    mpz_clear(scaling_factor);
    mpz_clear(modulo);
    gmp_randclear(state);
}

void test_encrypt_mult_cipher()
{
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    // Encoding
    printf("TEST ENCRYPTED MULTIPLICATION WITH CIPHER \n");
    size_t size = 2; // N = 2
    mpz_t encoding_precision_factor;
    mpz_init_set_ui(encoding_precision_factor, 512); // precision factor
    mpz_t scaling_factor;
    mpz_init_set_ui(scaling_factor, 1048576); // delta 2^20;
    mpz_t modulo;
    mpz_init_set_ui(modulo, INT64_MAX);
    mpz_mul(modulo, modulo, scaling_factor); // Q = INT_MAX * scaling_factor
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 0.3;
    complex_vector1->vector[1] = conjf(0.3);
    fprintf(stdout, "First vector : \n");
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = -0.3;
    complex_vector2->vector[1] = conjf(-0.3);
    fprintf(stdout, "Second vector : \n");
    complex_vector_print(complex_vector2);

    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "First encoded vector : \n");
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "Second encoded vector : \n");
    encoded_polynomial_print(encoded_pol2);

    // Encrypting and precomputing
    mpz_t g;
    mpz_init(g);
    big_power_of_2_below(g, modulo);
    gmp_printf("\n%Zd\n", g);
    polynomial_t *secret_key = key_generation(size, state);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);
    ciphered_t *c1 = encrypt(encoded_pol1, secret_key, modulo, scaling_factor, state);
    ciphered_t *c2 = encrypt(encoded_pol2, secret_key, modulo, scaling_factor, state);
    fprintf(stdout, "First ciphered vector : \n");
    cipher_print(c1);

    fprintf(stdout, "Second ciphered vector : \n");
    cipher_print(c2);

    ciphered_t *evk_g_precompute = evk_g(secret_key, modulo, g, state);
    ciphered_t *mult = ckks_mult_cipher_cipher(c1, c2, modulo, evk_g_precompute, g, scaling_factor);

    cipher_print(mult);

    encoded_polynomial_t *scaled_M = decrypt(mult, secret_key, modulo, scaling_factor);
    fprintf(stdout, "Mult after decryption : \n");
    encoded_polynomial_print(scaled_M);

    // Decoding
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    mpz_mul(encoding_precision_factor, encoding_precision_factor, encoding_precision_factor);
    complex_vector_t *recovered_complex_vector = recover_vector(scaled_M, basis_matrix_etoile, encoding_precision_factor);
    fprintf(stdout, "Recovered vector");
    complex_vector_print(recovered_complex_vector);
}

void tst_mult_consec()
{
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    // Encoding
    printf("TEST 3 ENCRYPTED MULTIPLICATION WITH CIPHER \n");
    size_t size = 2; // N = 2
    mpz_t encoding_precision_factor;
    mpz_init_set_ui(encoding_precision_factor, 128); // precision factor
    mpz_t scaling_factor;
    mpz_init_set_ui(scaling_factor, 2); // delta 2^30;
    mpz_t modulo;
    mpz_init_set_ui(modulo, INT32_MAX);
    mpz_mul(modulo, modulo, scaling_factor); // Q = INT_MAX * scaling_factor
    mpz_mul(modulo, modulo, scaling_factor); // Q = INT_MAX * scaling_factor^2
    complex_vector_t *complex_vector1 = complex_vector_init(size);
    complex_vector1->vector[0] = 3;
    complex_vector1->vector[1] = conjf(3);
    fprintf(stdout, "First vector : \n");
    complex_vector_print(complex_vector1);

    complex_vector_t *complex_vector2 = complex_vector_init(size);
    complex_vector2->vector[0] = 3;
    complex_vector2->vector[1] = conjf(3);
    fprintf(stdout, "Second vector : \n");
    complex_vector_print(complex_vector2);

    complex_vector_t *complex_vector3 = complex_vector_init(size);
    complex_vector3->vector[0] = 3;
    complex_vector3->vector[1] = conjf(3);
    fprintf(stdout, "Third vector : \n");
    complex_vector_print(complex_vector3);


    complex_matrix_t *mult_sigma_IN = mult_sigma_basis_anti_identity(size);
    encoded_polynomial_t *encoded_pol1 = complex_vector_encode(complex_vector1, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "First encoded vector : \n");
    encoded_polynomial_print(encoded_pol1);
    encoded_polynomial_t *encoded_pol2 = complex_vector_encode(complex_vector2, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "Second encoded vector : \n");
    encoded_polynomial_print(encoded_pol2);
    encoded_polynomial_t *encoded_pol3 = complex_vector_encode(complex_vector3, mult_sigma_IN, encoding_precision_factor);
    fprintf(stdout, "Third encoded vector : \n");
    encoded_polynomial_print(encoded_pol3);

    // Encrypting and precomputing
    mpz_t g;
    mpz_init(g);
    big_power_of_2_below(g, modulo);
    gmp_printf("\n%Zd\n", g);
    polynomial_t *secret_key = key_generation(size, state);

    fprintf(stdout, " Key : \n");
    polynomial_print(secret_key);
    ciphered_t *c1 = encrypt(encoded_pol1, secret_key, modulo, scaling_factor, state);
    ciphered_t *c2 = encrypt(encoded_pol2, secret_key, modulo, scaling_factor, state);
    ciphered_t *c3 = encrypt(encoded_pol3, secret_key, modulo, scaling_factor, state);
    fprintf(stdout, "First ciphered vector : \n");
    cipher_print(c1);

    fprintf(stdout, "Second ciphered vector : \n");
    cipher_print(c2);

    fprintf(stdout, "Second ciphered vector : \n");
    cipher_print(c3);

    ciphered_t *evk_g_precompute = evk_g(secret_key, modulo, g, state);
    ciphered_t *mult = ckks_mult_cipher_cipher(c1, c2, modulo, evk_g_precompute, g, scaling_factor); //modifying modulo inside
    cipher_print(mult);
    ciphered_t *evk_g_precompute2 = evk_g(secret_key, modulo, g, state);
    ciphered_t *mult2 = ckks_mult_cipher_cipher(mult, c3, modulo, evk_g_precompute2, g, scaling_factor);
    cipher_print(mult2);

    encoded_polynomial_t *scaled_M = decrypt(mult2, secret_key, modulo, scaling_factor);
    fprintf(stdout, "Mult after decryption : \n");
    encoded_polynomial_print(scaled_M);

    // Decoding
    complex_matrix_t *basis_matrix_etoile = sigma_basis_tilde_etoile_init(size);
    mpz_t cumult_precision_factor;
    mpz_init_set(cumult_precision_factor, encoding_precision_factor);
    mpz_mul(cumult_precision_factor, cumult_precision_factor, encoding_precision_factor);
    mpz_mul(cumult_precision_factor, cumult_precision_factor, encoding_precision_factor);
    complex_vector_t *recovered_complex_vector = recover_vector(scaled_M, basis_matrix_etoile, cumult_precision_factor);
    fprintf(stdout, "Recovered vector");
    complex_vector_print(recovered_complex_vector);
}