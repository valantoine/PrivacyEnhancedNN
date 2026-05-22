#include "polynomial_operations.h"

void pol_add(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]);
    }
}



// Mult must be in [-q/2, q/2]
encoded_polynomial_t *pol_mult(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, uint64_t scaling_factor)
{
    encoded_polynomial_t *mult_pol = encoded_pol_init(pol1->size);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < mult_pol->size; i++)
    {
        for (size_t j = 0; j < mult_pol->size; j++)
        {
            if ((i + j) >= mult_pol->size)
            {
                mult_pol->coeffs[(i + j) % mult_pol->size] += (pol1->coeffs[i] * pol2->coeffs[j] * (-1)) / (int64_t)(scaling_factor);
            }
            else
            {
                mult_pol->coeffs[(i + j)] += (pol1->coeffs[i] * pol2->coeffs[j]) / (int64_t)(scaling_factor);
            }
        }
        
    }

    return mult_pol;
}
