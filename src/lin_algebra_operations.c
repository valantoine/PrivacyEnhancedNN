#include "lin_algebra_operations.h"

// return : matrix1 = matrix1 + matrix2
void int_matrix_add(int_matrix_t *matrix1, const int_matrix_t *matrix2)
{
    if (matrix1->nb_rows != matrix2->nb_rows || matrix1->nb_cols != matrix2->nb_cols)
    {

        fprintf(stderr, "lin_algebra_operations : matrix not the same size error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < matrix1->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix1->nb_cols; j++)
        {
            int_matrix_set_cell(matrix1, i, j, int_matrix_get_cell(matrix1, i, j) + int_matrix_get_cell(matrix2, i, j));
        }
    }
}

// return : matrix1 = matrix1 - matrix2
void int_matrix_sub(int_matrix_t *matrix1, const int_matrix_t *matrix2)
{
    if (matrix1->nb_rows != matrix2->nb_rows || matrix1->nb_cols != matrix2->nb_cols)
    {

        fprintf(stderr, "lin_algebra_operations : matrix not the same size error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < matrix1->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix1->nb_cols; j++)
        {
            int_matrix_set_cell(matrix1, i, j, int_matrix_get_cell(matrix1, i, j) - int_matrix_get_cell(matrix2, i, j));
        }
    }
}

// return matrix3 = matrix1 * matrix2
int_matrix_t int_matrix_mult(const int_matrix_t *matrix1, const int_matrix_t *matrix2)
{
    if (matrix1->nb_cols != matrix2->nb_rows)
    {
        fprintf(stderr, "lin_algebra_operations : dimension error for multiplication");
        exit(EXIT_FAILURE);
    }
    int_matrix_t res;
    int_matrix_allocate(&res, matrix1->nb_rows, matrix2->nb_cols);
    for (size_t i = 0; i < matrix1->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix2->nb_cols; j++)
        {
            int64_t sum = 0;
            for (size_t k = 0; k < matrix1->nb_cols; k++)
            {
                sum += int_matrix_get_cell(matrix1, i, k) + int_matrix_get_cell(matrix2, k, j);
            }
            int_matrix_set_cell(&res, i, j, sum);
        }
    }
    return res;
}

// Float matrix part

// return : matrix1 = matrix1 + matrix2
void float_matrix_add(float_matrix_t *matrix1, const float_matrix_t *matrix2)
{
    if (matrix1->nb_rows != matrix2->nb_rows || matrix1->nb_cols != matrix2->nb_cols)
    {

        fprintf(stderr, "lin_algebra_operations : matrix not the same size error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < matrix1->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix1->nb_cols; j++)
        {
            float_matrix_set_cell(matrix1, i, j, float_matrix_get_cell(matrix1, i, j) + float_matrix_get_cell(matrix2, i, j));
        }
    }
}

// return : matrix1 = matrix1 - matrix2
void float_matrix_sub(float_matrix_t *matrix1, const float_matrix_t *matrix2)
{
    if (matrix1->nb_rows != matrix2->nb_rows || matrix1->nb_cols != matrix2->nb_cols)
    {

        fprintf(stderr, "lin_algebra_operations : matrix not the same size error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < matrix1->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix1->nb_cols; j++)
        {
            float_matrix_set_cell(matrix1, i, j, float_matrix_get_cell(matrix1, i, j) - float_matrix_get_cell(matrix2, i, j));
        }
    }
}

// return matrix3 = matrix1 * matrix2
float_matrix_t float_matrix_mult(const float_matrix_t *matrix1, const float_matrix_t *matrix2)
{
    if (matrix1->nb_cols != matrix2->nb_rows)
    {
        fprintf(stderr, "lin_algebra_operations : dimension error for multiplication");
        exit(EXIT_FAILURE);
    }
    float_matrix_t res;
    float_matrix_allocate(&res, matrix1->nb_rows, matrix2->nb_cols);
    for (size_t i = 0; i < matrix1->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix2->nb_cols; j++)
        {
            float sum = 0;
            for (size_t k = 0; k < matrix1->nb_cols; k++)
            {
                sum += float_matrix_get_cell(matrix1, i, k) + float_matrix_get_cell(matrix2, k, j);
            }
            float_matrix_set_cell(&res, i, j, sum);
        }
    }
    return res;
}
