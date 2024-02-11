#ifndef _MATH_H
#define _MATH_H 1

#include "../include/stdint.h"
static uint32_t seed = 0;

#define RAND_MAX 4294967295 // 2^32 - 1
#define NORM 2.328306549295728e-10

int rand();
void srand();
#define PI 3.14159265358979323846
#define EULER 2.71828182845904523536
#define EPS 1.073741824e-16
#define typename(x) _Generic((x),                                      \
                             short int                                 \
                             : "short int", unsigned short int         \
                             : "unsigned short int",                   \
                               int                                     \
                             : "int", unsigned int                     \
                             : "unsigned int",                         \
                               long int                                \
                             : "long int", unsigned long int           \
                             : "unsigned long int",                    \
                               long long int                           \
                             : "long long int", unsigned long long int \
                             : "unsigned long long int",               \
                               float                                   \
                             : "float", double                         \
                             : "double",                               \
                               long double                             \
                             : "long double",                          \
                               void *                                  \
                             : "pointer to void", int *                \
                             : "pointer to int",                       \
                               default                                 \
                             : "other")

double machine_eps(double x);
uint32_t fact(uint32_t n);
int abs(int n);
double ipow(double x, uint32_t n);
double pow(double x, double y);
double sqrt(double x);
double inv_sqrt(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double sinh(double x);
double cosh(double x);
double tanh(double x);
double asinh(double x);
double acosh(double x);
double atanh(double x);
double exp(double x);
double ln(double x);
uint32_t log2(uint32_t x);
double log10(double x);
double log(double x, double y);
int fac(unsigned int x);
int small_rand();
double fmax(double x, double y);
double fabs(double x);
#endif