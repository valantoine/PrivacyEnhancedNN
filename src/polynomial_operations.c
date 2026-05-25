#include "polynomial_operations.h"
void mpz_center_mod(mpz_t result, mpz_t a, mpz_t q)
{
    mpz_mod(result, a, q); // result in [0, q)
    mpz_t half_q;
    mpz_init(half_q);
    mpz_tdiv_q_2exp(half_q, q, 1); // half_q = q/2
    if (mpz_cmp(result, half_q) > 0)
        mpz_sub(result, result, q); // result -= q
    mpz_clear(half_q);
}

polynomial_t *polynomial_init(size_t size)
{
    polynomial_t *pol = malloc(sizeof(polynomial_t));
    if (pol == NULL)
    {

        fprintf(stderr, "encoding : Failed to allocate");
        return NULL;
    }
    pol->coeffs = malloc(sizeof(mpz_t) * size);
    if (pol->coeffs == NULL)
    {

        fprintf(stderr, "encoding : Failed to allocate");
        free(pol);
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        mpz_init(pol->coeffs[i]);
    }
    pol->degree = size;
    return pol;
}

void polynomial_init_in_place(polynomial_t *pol, size_t size)
{
    pol->coeffs = malloc(sizeof(mpz_t) * size);
    if (pol->coeffs == NULL)
    {
        fprintf(stderr, "encoding : Failed to allocate");
        free(pol);
        return;
    }
    for (size_t i = 0; i < size; i++)
    {
        mpz_init(pol->coeffs[i]); // bored to error handling this
    }
    pol->degree = size;
    return;
}

void polynomial_free(polynomial_t *pol)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        mpz_clear(pol->coeffs[i]);
    }
    free(pol->coeffs);
    free(pol);
}

void polynomial_print(polynomial_t *pol)
{
    for (size_t i = 0; i < pol->degree - 1; i++)
    {
        gmp_printf("%Zd *X^%zu + ", pol->coeffs[i], i);
    }
    gmp_printf("%Zd *X^%zu", pol->coeffs[pol->degree - 1], pol->degree - 1);
}

void encoded_pol_add(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        mpz_add(pol1->coeffs[i], pol1->coeffs[i], pol2->coeffs[i]);
    }
}

void encoded_pol_add_modulo(encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t modulo)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        mpz_add(pol1->coeffs[i], pol1->coeffs[i], pol2->coeffs[i]);
        mpz_center_mod(pol1->coeffs[i], pol1->coeffs[i], modulo);
    }
}

// Mult must be in [-q/2, q/2]
encoded_polynomial_t *encoded_pol_mult_rescaled(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t scaling_factor)
{
    encoded_polynomial_t *mult_pol = encoded_pol_init(pol1->size);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->size; i++)
    {
        for (size_t j = 0; j < mult_pol->size; j++)
        {

            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            mpz_divexact(temp, temp, scaling_factor); // since the product is scaling_factor * pol1 * scaling_factor * pol2, we can divide exactly a scaling factor.
            if ((i + j) >= mult_pol->size)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->size], mult_pol->coeffs[(i + j) % mult_pol->size], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

encoded_polynomial_t *encoded_pol_mult_rescaled_modulo(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t scaling_factor, mpz_t modulo)
{
    encoded_polynomial_t *mult_pol = encoded_pol_init(pol1->size);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->size; i++)
    {
        for (size_t j = 0; j < mult_pol->size; j++)
        {
            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            mpz_divexact(temp, temp, scaling_factor); // since the product is scaling_factor * pol1 * scaling_factor * pol2, we can divide exactly a scaling factor.
            mpz_center_mod(temp, temp, modulo);
            if ((i + j) >= mult_pol->size)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->size], mult_pol->coeffs[(i + j) % mult_pol->size], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

void encoded_pol_add_polynomial(encoded_polynomial_t *pol1, const polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        mpz_add(pol1->coeffs[i], pol1->coeffs[i], pol2->coeffs[i]);
    }
}

void encoded_pol_add_polynomial_modulo(encoded_polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo)
{
    for (size_t i = 0; i < pol1->size; i++)
    {
        mpz_add(pol1->coeffs[i], pol1->coeffs[i], pol2->coeffs[i]);
        mpz_center_mod(pol1->coeffs[i], pol1->coeffs[i], modulo);
    }
}

encoded_polynomial_t *encoded_pol_mult_polynomial_modulo(const encoded_polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo)
{
    encoded_polynomial_t *mult_pol = encoded_pol_init(pol1->size);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->size; i++)
    {
        for (size_t j = 0; j < mult_pol->size; j++)
        {
            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            mpz_center_mod(temp, temp, modulo);
            if ((i + j) >= mult_pol->size)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->size], mult_pol->coeffs[(i + j) % mult_pol->size], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

void encoded_pol_scalar_mult(encoded_polynomial_t *pol, mpz_t scalar)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        mpz_mul(pol->coeffs[i], pol->coeffs[i], scalar);
    }
}

void encoded_pol_scalar_div_exact(encoded_polynomial_t *scaled_pol, mpz_t scalar)
{
    // all coeff should be divisible by scalar.
    for (size_t i = 0; i < scaled_pol->size; i++)
    {
        mpz_divexact(scaled_pol->coeffs[i], scaled_pol->coeffs[i], scalar);
    }
}

void encoded_pol_scalar_mult_modulo(encoded_polynomial_t *pol, mpz_t scalar, mpz_t modulo)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        mpz_mul(pol->coeffs[i], pol->coeffs[i], scalar);
        mpz_center_mod(pol->coeffs[i], pol->coeffs[i], modulo);
    }
}

polynomial_t *polynomial_mult(const polynomial_t *pol1, const polynomial_t *pol2)
{
    polynomial_t *mult_pol = polynomial_init(pol1->degree);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->degree; i++)
    {
        for (size_t j = 0; j < mult_pol->degree; j++)
        {
            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            if ((i + j) >= mult_pol->degree)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->degree], mult_pol->coeffs[(i + j) % mult_pol->degree], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

polynomial_t *polynomial_mult_modulo(const polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo)
{
    polynomial_t *mult_pol = polynomial_init(pol1->degree);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->degree; i++)
    {
        for (size_t j = 0; j < mult_pol->degree; j++)
        {
            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            mpz_center_mod(temp, temp, modulo);
            if ((i + j) >= mult_pol->degree)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->degree], mult_pol->coeffs[(i + j) % mult_pol->degree], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

void polynomial_scalar_mult(polynomial_t *pol, mpz_t scalar)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        mpz_mul(pol->coeffs[i], pol->coeffs[i], scalar);
    }
}

void polynomial_scalar_mult_modulo(polynomial_t *pol, mpz_t scalar, mpz_t modulo)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        mpz_mul(pol->coeffs[i], pol->coeffs[i], scalar);
        mpz_center_mod(pol->coeffs[i], pol->coeffs[i], modulo);
    }
}

// Rescaling function to decrease the mod Q of one multiplicative level after
void div_and_round_polynomial(polynomial_t *pol, mpz_t divisor)
{
    mpz_t half, temp;
    mpz_init(temp);
    mpz_init(half);
    mpz_tdiv_q_2exp(half, divisor, 1); // half = divisor / 2
    for (size_t i = 0; i < pol->degree; i++)
    {
        if (mpz_sgn(pol->coeffs[i]) >= 0)
        {
            // (x + divisor/2) / divisor
            mpz_add(temp, pol->coeffs[i], half);
            mpz_tdiv_q(pol->coeffs[i], temp, divisor); // truncate
        }
        else
        {
            // -(-x + divisor/2) / divisor
            mpz_neg(temp, pol->coeffs[i]);   // temp = -x
            mpz_add(temp, temp, half);       // temp = -x + divisor/2
            mpz_tdiv_q(temp, temp, divisor); // temp = (-x + divisor/2) / divisor
            mpz_neg(pol->coeffs[i], temp);   // coeffs = -temp
        }
    }

    mpz_clear(temp);
    mpz_clear(half);
}

void div_and_round_encoded_polynomial(encoded_polynomial_t *pol, mpz_t divisor)
{
    mpz_t half, temp;
    mpz_init(temp);
    mpz_init(half);
    mpz_tdiv_q_2exp(half, divisor, 1); // half = divisor / 2
    for (size_t i = 0; i < pol->size; i++)
    {
        if (mpz_sgn(pol->coeffs[i]) >= 0)
        {
            // (x + divisor/2) / divisor
            mpz_add(temp, pol->coeffs[i], half);
            mpz_tdiv_q(pol->coeffs[i], temp, divisor); // truncate
        }
        else
        {
            // -(-x + divisor/2) / divisor
            mpz_neg(temp, pol->coeffs[i]);   // temp = -x
            mpz_add(temp, temp, half);       // temp = -x + divisor/2
            mpz_tdiv_q(temp, temp, divisor); // temp = (-x + divisor/2) / divisor
            mpz_neg(pol->coeffs[i], temp);   // coeffs = -temp
        }
    }

    mpz_clear(temp);
    mpz_clear(half);
}

encoded_polynomial_t *encoded_pol_mult_modulo_no_rescaling(const encoded_polynomial_t *pol1, const encoded_polynomial_t *pol2, mpz_t modulo)
{
    encoded_polynomial_t *mult_pol = encoded_pol_init(pol1->size);
    if (mult_pol == NULL)
    {
        return NULL;
    }
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->size; i++)
    {
        for (size_t j = 0; j < mult_pol->size; j++)
        {
            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            mpz_center_mod(temp, temp, modulo);
            if ((i + j) >= mult_pol->size)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->size], mult_pol->coeffs[(i + j) % mult_pol->size], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

polynomial_t *encoded_to_polynomial(encoded_polynomial_t *encoded)
{
    polynomial_t *p = polynomial_init(encoded->size);
    if (p == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < encoded->size; i++)
    {
        mpz_set(p->coeffs[i], encoded->coeffs[i]);
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
    mpz_t temp;
    mpz_init(temp);
    for (size_t i = 0; i < mult_pol->size; i++)
    {
        for (size_t j = 0; j < mult_pol->size; j++)
        {
            mpz_mul(temp, pol1->coeffs[i], pol2->coeffs[j]);
            if ((i + j) >= mult_pol->size)
            {
                mpz_neg(temp, temp);
                mpz_add(mult_pol->coeffs[(i + j) % mult_pol->size], mult_pol->coeffs[(i + j) % mult_pol->size], temp);
            }
            else
            {
                mpz_add(mult_pol->coeffs[(i + j)], mult_pol->coeffs[(i + j)], temp);
            }
        }
    }
    mpz_clear(temp);
    return mult_pol;
}

void polynomial_add(polynomial_t *pol1, const polynomial_t *pol2)
{
    for (size_t i = 0; i < pol1->degree; i++)
    {
        mpz_add(pol1->coeffs[i], pol1->coeffs[i], pol2->coeffs[i]);
    }
}

void polynomial_add_modulo(polynomial_t *pol1, const polynomial_t *pol2, mpz_t modulo)
{
    for (size_t i = 0; i < pol1->degree; i++)
    {
        mpz_add(pol1->coeffs[i], pol1->coeffs[i], pol2->coeffs[i]);
        mpz_center_mod(pol1->coeffs[i], pol1->coeffs[i], modulo);
    }
}

void polynomial_eval_modulo(polynomial_t *pol, mpz_t modulo)
{
    for (size_t i = 0; i < pol->degree; i++)
    {
        mpz_center_mod(pol->coeffs[i], pol->coeffs[i], modulo);
    }
}

void encoded_polynomial_eval_modulo(encoded_polynomial_t *pol, mpz_t modulo)
{
    for (size_t i = 0; i < pol->size; i++)
    {
        mpz_center_mod(pol->coeffs[i], pol->coeffs[i], modulo);
    }
}