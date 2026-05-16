#ifndef LIN_ALGEBRA_MISC_H
#define LIN_ALGEBRA_MISC_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

typedef struct
{
    int64_t *cells;
    size_t nb_rows;
    size_t nb_cols;
} int_matrix_t;


// Initialize to null matrix (filled with zeros)
extern void int_matrix_allocate(int_matrix_t *matrix, const size_t nb_rows, const size_t nb_cols);

extern void int_matrix_free(const int_matrix_t *matrix);

extern int64_t int_matrix_get_cell(const int_matrix_t *matrix, const size_t i, const size_t j);

extern void int_matrix_set_cell(int_matrix_t *matrix, const size_t i, const size_t j, const int64_t element);

extern void int_matrix_print(const int_matrix_t *matrix, FILE *fd);

typedef struct
{
    float *cells;
    size_t nb_rows;
    size_t nb_cols;
} float_matrix_t;

// Initialize to null matrix (filled with zeros)
extern void float_matrix_allocate(float_matrix_t *matrix, const size_t nb_rows, const size_t nb_cols);

extern void float_matrix_free(const float_matrix_t *matrix);

extern float float_matrix_get_cell(const float_matrix_t *matrix, const size_t i, const size_t j);

extern void float_matrix_set_cell(float_matrix_t *matrix, const size_t i, const size_t j, const float element);

extern void float_matrix_print(const float_matrix_t *matrix, FILE *fd);


#endif /*LIN_ALGEBRA_MISC_H*/