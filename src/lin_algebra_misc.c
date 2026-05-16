#include "lin_algebra_misc.h"

void int_matrix_allocate(int_matrix_t *matrix, const size_t nb_rows, const size_t nb_cols)
{
    if (nb_rows == 0 || nb_cols == 0)
    {
        fprintf(stderr, "lin_algebra_misc : dimension error while allocating");
        exit(EXIT_FAILURE);
    }
    matrix->nb_rows = nb_rows;
    matrix->nb_cols = nb_cols;
    matrix->cells = (int64_t *)calloc(nb_rows * nb_cols, sizeof(int64_t));
    if (matrix->cells == NULL)
    {
        fprintf(stderr, "lin_algebra_misc : allocation error");
        exit(EXIT_FAILURE);
    }
}

void int_matrix_free(const int_matrix_t *matrix)
{
    if (matrix->cells == NULL)
    {
        fprintf(stderr, "lin_algebra_misc : free error");
        exit(EXIT_FAILURE);
    }
    free(matrix->cells);
}

int64_t int_matrix_get_cell(const int_matrix_t *matrix, const size_t i, const size_t j)
{
    if (i >= matrix->nb_rows || j >= matrix->nb_cols)
    {
        fprintf(stderr, "lin_algebra_misc : int_matrix_get_cell error size");
        exit(EXIT_FAILURE);
    }

    return matrix->cells[i * matrix->nb_cols + j];
}

void int_matrix_set_cell(int_matrix_t *matrix, const size_t i, const size_t j, const int64_t element)
{
    if (i >= matrix->nb_rows || j >= matrix->nb_cols)
    {
        fprintf(stderr, "lin_algebra_misc : int_matrix_set_cell error size");
        exit(EXIT_FAILURE);
    }
    matrix->cells[i * matrix->nb_cols + j] = element;
}

void int_matrix_print(const int_matrix_t *matrix, FILE *fd)
{
    for (size_t i = 0; i < matrix->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix->nb_cols; j++)
        {
            fprintf(fd, "%" PRId64" ", int_matrix_get_cell(matrix, i, j));
        }
        fprintf(fd, "\n");
    }
    fprintf(fd, "\n");
}

//Float matrix part

void float_matrix_allocate(float_matrix_t *matrix, const size_t nb_rows, const size_t nb_cols)
{
    if (nb_rows == 0 || nb_cols == 0)
    {
        fprintf(stderr, "lin_algebra_misc : dimension error while allocating");
        exit(EXIT_FAILURE);
    }
    matrix->nb_rows = nb_rows;
    matrix->nb_cols = nb_cols;
    matrix->cells = (float *)calloc(nb_rows * nb_cols, sizeof(float));
    if (matrix->cells == NULL)
    {
        fprintf(stderr, "lin_algebra_misc : allocation error");
        exit(EXIT_FAILURE);
    }
}

void float_matrix_free(const float_matrix_t *matrix)
{
    if (matrix->cells == NULL)
    {
        fprintf(stderr, "lin_algebra_misc : free error");
        exit(EXIT_FAILURE);
    }
    free(matrix->cells);
}

float float_matrix_get_cell(const float_matrix_t *matrix, const size_t i, const size_t j)
{
    if (i >= matrix->nb_rows || j >= matrix->nb_cols)
    {
        fprintf(stderr, "lin_algebra_misc : float_matrix_get_cell error size");
        exit(EXIT_FAILURE);
    }

    return matrix->cells[i * matrix->nb_cols + j];
}

void float_matrix_set_cell(float_matrix_t *matrix, const size_t i, const size_t j, const float element)
{
    if (i >= matrix->nb_rows || j >= matrix->nb_cols)
    {
        fprintf(stderr, "lin_algebra_misc : float_matrix_set_cell error size");
        exit(EXIT_FAILURE);
    }
    matrix->cells[i * matrix->nb_cols + j] = element;
}

void float_matrix_print(const float_matrix_t *matrix, FILE *fd)
{
    for (size_t i = 0; i < matrix->nb_rows; i++)
    {
        for (size_t j = 0; j < matrix->nb_cols; j++)
        {
            fprintf(fd, "%lf ", float_matrix_get_cell(matrix, i, j));
        }
        fprintf(fd, "\n");
    }
    fprintf(fd, "\n");
}