#include "polynomial_operations.h"

polynomial_t *polyonimal_init(size_t size)
{
    polynomial_t *pol = malloc(sizeof(polynomial_t));
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
    pol->degree = size;
    return pol;
}

void polynomial_free(polynomial_t *pol)
{
    free(pol->coeffs);
    free(pol);
}

void polynomial_print(polynomial_t *pol)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        fprintf(stdout, "%" PRId64 "*X^%zu + ", pol->coeffs[i], i);
    }
    fprintf(stdout, "\n");
}

void encoded_pol_add(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]);
    }
}

void encoded_pol_add_modulo(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t modulo)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]) % modulo;
    }
}

// Mult must be in [-q/2, q/2]
encoded_polynomial_t *encoded_pol_mult(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t scaling_factor)
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

encoded_polynomial_t *encoded_pol_mult_modulo(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t scaling_factor, int64_t modulo)
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
                mult_pol->coeffs[(i + j) % mult_pol->size] += ((pol1->coeffs[i] * pol2->coeffs[j] * (-1)) / (int64_t)(scaling_factor)) % modulo;
            }
            else
            {
                mult_pol->coeffs[(i + j)] += ((pol1->coeffs[i] * pol2->coeffs[j]) / (int64_t)(scaling_factor)) % modulo;
            }
        }
    }

    return mult_pol;
}

void encoded_pol_add_polynomial(encoded_polynomial_t *pol1, const polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]);
    }
}

void encoded_pol_add_polynomial_modulo(encoded_polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]) % modulo;
    }
}

encoded_polynomial_t *encoded_pol_mult_polynomial_modulo(const encoded_polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo)
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
                mult_pol->coeffs[(i + j) % mult_pol->size] += ((pol1->coeffs[i] * pol2->coeffs[j] * (-1))) % modulo;
            }
            else
            {
                mult_pol->coeffs[(i + j)] += ((pol1->coeffs[i] * pol2->coeffs[j])) % modulo;
            }
        }
    }

    return mult_pol;
}

void encoded_pol_scalar_mult(encoded_polynomial_t *pol, int64_t scalar)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        pol->coeffs[i] = (pol->coeffs[i] * scalar);
    }
}

void encoded_pol_scalar_div(encoded_polynomial_t *scaled_pol, int64_t scalar)
{
    // all coeff should be divisible by scalar.
    for (size_t i = 0; i < scaled_pol->size; i++)
    {
        scaled_pol->coeffs[i] = (scaled_pol->coeffs[i] / scalar);
    }
}

void encoded_pol_scalar_mult_modulo(encoded_polynomial_t *pol, int64_t scalar, int64_t modulo)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        pol->coeffs[i] = (pol->coeffs[i] * scalar) % modulo;
    }
}

polynomial_t *polynomial_mult(const polynomial_t *pol1, const polynomial_t *pol2)
{
    polynomial_t *mult_pol = polyonimal_init(pol1->degree);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < mult_pol->degree; i++)
    {
        for (size_t j = 0; j < mult_pol->degree; j++)
        {
            if ((i + j) >= mult_pol->degree)
            {
                mult_pol->coeffs[(i + j) % mult_pol->degree] += (pol1->coeffs[i] * pol2->coeffs[j] * (-1));
            }
            else
            {
                mult_pol->coeffs[(i + j)] += (pol1->coeffs[i] * pol2->coeffs[j]);
            }
        }
    }
    return mult_pol;
}

polynomial_t *polynomial_mult_modulo(const polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo)
{
    polynomial_t *mult_pol = polyonimal_init(pol1->degree);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < mult_pol->degree; i++)
    {
        for (size_t j = 0; j < mult_pol->degree; j++)
        {
            if ((i + j) >= mult_pol->degree)
            {
                mult_pol->coeffs[(i + j) % mult_pol->degree] += (pol1->coeffs[i] * pol2->coeffs[j] * (-1)) % modulo;
            }
            else
            {
                mult_pol->coeffs[(i + j)] += (pol1->coeffs[i] * pol2->coeffs[j]) % modulo;
            }
        }
    }
    return mult_pol;
}

void polynomial_scalar_mult(polynomial_t *pol, int64_t scalar)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        pol->coeffs[i] = (pol->coeffs[i] * scalar);
    }
}

void polynomial_scalar_mult_modulo(polynomial_t *pol, int64_t scalar, int64_t modulo)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        pol->coeffs[i] = (pol->coeffs[i] * scalar) % modulo;
    }
}

// Rescaling function to decrease the mod Q of one multiplicative level after
void div_and_round_polynomial(polynomial_t *pol, int64_t divisor)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        int64_t x = pol->coeffs[i];
        if (x >= 0)
            pol->coeffs[i] = (x + divisor / 2) / divisor;
        else
            pol->coeffs[i] = -(-x + divisor / 2) / divisor;
    }
}

void div_and_round_encoded_polynomial(encoded_polynomial_t *pol, int64_t divisor)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        int64_t x = pol->coeffs[i];
        if (x >= 0)
            pol->coeffs[i] = (x + divisor / 2) / divisor;
        else
            pol->coeffs[i] = -(-x + divisor / 2) / divisor;
    }
}

encoded_polynomial_t *encoded_pol_mult_modulo_no_rescaling(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, int64_t modulo)
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
                mult_pol->coeffs[(i + j) % mult_pol->size] += (pol1->coeffs[i] * pol2->coeffs[j] * (-1)) % modulo;
            }
            else
            {
                mult_pol->coeffs[(i + j)] += (pol1->coeffs[i] * pol2->coeffs[j]) % modulo;
            }
        }
    }

    return mult_pol;
}

polynomial_t *encoded_to_polynomial(encoded_polynomial_t *encoded)
{
    polynomial_t *p = polyonimal_init(encoded->size);
    if (p == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < encoded->size; i++)
    {
        p->coeffs[i] = encoded->coeffs[i];
    }
    return p;
}

encoded_polynomial_t *encoded_pol_mult_polynomial(const encoded_polynomial_t *pol1, const polynomial_t *pol2)
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
                mult_pol->coeffs[(i + j) % mult_pol->size] += ((pol1->coeffs[i] * pol2->coeffs[j] * (-1)));
            }
            else
            {
                mult_pol->coeffs[(i + j)] += ((pol1->coeffs[i] * pol2->coeffs[j]));
            }
        }
    }

    return mult_pol;
}

void polynomial_add(polynomial_t *pol1, const polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->degree; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]);
    }
}

void polynomial_add_modulo(polynomial_t *pol1, const polynomial_t *pol2, int64_t modulo)
{
    for (size_t i = 0; i < pol1->degree; i++)
    {
        pol1->coeffs[i] = (pol1->coeffs[i] + pol2->coeffs[i]) % modulo;
    }
}