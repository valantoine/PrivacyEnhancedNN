#ifndef CKKS_ENCODING_H
#define CKKS_ENCODING_H
#include "encoding_misc.h"
#include "polynomial_operations.h"
#include <gmp.h> //used for later with big integers
#ifndef  M_PI
#define  M_PI  3.1415926535897932384626433
#endif

extern void test_encode_decode();

extern void test_add_encode_decode();

extern void test_mult_encode_decode();

#endif /*CKKS_ENCODING_H*/