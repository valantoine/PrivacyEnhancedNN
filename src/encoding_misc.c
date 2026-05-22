#include "encoding_misc.h"


complex_vector_t *complex_vector_init(size_t size)
{
    complex_vector_t *cv = malloc(sizeof(complex_vector_t));
    if (cv == NULL)
    {
        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    cv->vector = calloc(size, sizeof(float complex) * size);
    if (cv->vector == NULL)
    {
        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    cv->size = size;
    return cv;
}

void complex_vector_free(complex_vector_t *cv)
{
    free(cv->vector);
    free(cv);
}

complex_matrix_t *complex_matrix_init(size_t size)
{
    complex_matrix_t *cm = malloc(sizeof(complex_matrix_t));
    if (cm == NULL)
    {
        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    cm->matrix = calloc(size * size, sizeof(float complex) * size * size);
    if (cm->matrix == NULL)
    {
        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    cm->size = size;
    return cm;
}

void complex_matrix_free(complex_matrix_t *cm)
{
    free(cm->matrix);
    free(cm);
}

encoded_polynomial_t *encoded_pol_init(size_t size)
{
    encoded_polynomial_t *pol = malloc(sizeof(encoded_polynomial_t));
    if (pol == NULL)
    {

        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    pol->coeffs = calloc(size, sizeof(int64_t) * size);
    if (pol->coeffs == NULL)
    {

        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    pol->size = size;
    return pol;
}

void encoded_pol_free(encoded_polynomial_t *pol)
{
    free(pol->coeffs);
    free(pol);
}

// to correct
float complex complex_matrix_get(complex_matrix_t *cm, uint16_t i, uint16_t j)
{
    return cm->matrix[i * cm->size + j];
}

void complex_matrix_set(complex_matrix_t *cm, uint16_t i, uint16_t j, float complex z)
{
    cm->matrix[i * cm->size + j] = z;
}

void polynomial_print(encoded_polynomial_t *pol)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        fprintf(stdout, "Coef %zu : %" PRId64 "\n", i, pol->coeffs[i]);
    }
}

void float_complex_print(float complex z)
{
    fprintf(stdout, "%f + I*%f ", crealf(z), cimagf(z));
}



void complex_matrix_print(complex_matrix_t *complex_matrix)
{
    for (size_t i = 0; i < complex_matrix->size; i++)
    {
        fprintf(stdout, "\n");
        for (size_t j = 0; j < complex_matrix->size; j++)
        {
            float_complex_print(complex_matrix_get(complex_matrix, i, j));
        }
    }
    fprintf(stdout, "\n");
}

void complex_vector_print(complex_vector_t *vector)
{
    fprintf(stdout, "Complex vector : {");
    for (size_t i = 0; i < vector->size; i++)
    {
        fprintf(stdout, " %f + I* %f,", crealf(vector->vector[i]), cimagf(vector->vector[i]));
    }
    fprintf(stdout, "}\n");
}