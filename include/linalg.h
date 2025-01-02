#ifndef LINALG_H
#define LINALG_H

#include <stdio.h>

#include "types.h"

void outer_product(const float list1[], const float list2[], float result[], int n);

void add(const float list1[], const float list2[], float result[], int n);

void add_three(const float list1[], const float list2[], const float list3[],float result[], int n);

void transpose(const float list[], float result[], int n);

float trace(const float list[], int n);

float det3(const float list[]);

void inverse3(const float list[], float result[]);

void scalar_multiply(const float list[], float scalar, float result[], int n);

float dot(const float list1[], const float list2[], int n);

float dot_coords(coords_3d_t list1, coords_3d_t list2);

float change_of_base(const float list1[], const float list2[], int n);

void matrix_multiply(const float list1[], const float list2[], float result[], int n);

void matrix_vector_multiply(const float list1[], const float list2[], float result[], int n);

coords_3d_t cross_product(coords_3d_t v1, coords_3d_t v2);

#endif