#ifndef POLYNOMIAL_OPERATIONS_H
#define POLYNOMIAL_OPERATIONS_H
#include "encoding_misc.h"



extern void pol_add(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2);

extern encoded_polynomial_t *pol_mult(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, uint64_t scaling_factor);



#endif /*POLYNOMIAL_OPERATIONS_H*/