#ifndef LIN_ALGEBRA_OPERATIONS_H
#define LIN_ALGEBRA_OPERATIONS_H
#include "lin_algebra_misc.h"


// return : matrix1 = matrix1 + matrix2
extern void int_matrix_add(int_matrix_t *matrix1, const int_matrix_t *matrix2);

// return : matrix1 = matrix1 - matrix2
extern void int_matrix_sub(int_matrix_t *matrix1, const int_matrix_t *matrix2);

// return matrix3 = matrix1 * matrix2
extern int_matrix_t int_matrix_mult(const int_matrix_t *matrix1, const int_matrix_t *matrix2);

// return : matrix1 = matrix1 + matrix2
extern void float_matrix_add(float_matrix_t *matrix1, const float_matrix_t *matrix2);

// return : matrix1 = matrix1 - matrix2
extern void float_matrix_sub(float_matrix_t *matrix1, const float_matrix_t *matrix2);

// return matrix3 = matrix1 * matrix2
extern float_matrix_t float_matrix_mult(const float_matrix_t *matrix1, const float_matrix_t *matrix2);




#endif /*LIN_ALGEBRA_OPERATIONS_H*/