#include <stdio.h>

void outer_product(const float list1[], const float list2[], float result[], int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i * n + j] = list1[i] * list2[j];
        }
    }
}

void add(const float list1[], const float list2[], float result[], int n) {
    for (int i = 0; i < n; ++i) {
        result[i] = list1[i] + list2[i];
    }
}

void add_three(const float list1[], const float list2[], const float list3[],float result[], int n) {
    for (int i = 0; i < n; ++i) {
        result[i] = list1[i] + list2[i] + list3[i];
    }
}

void transpose(const float list[], float result[], int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[j * n + i] = list[i * n + j];
        }
    }
}

float trace(const float list[], int n) {
    float sum = 0.f;
    for (int i = 0; i < n; ++i) {
        sum += list[i * n + i];
    }

    return sum;
}

float det3(const float list[]) {
    return list[0] * (list[4] * list[8] - list[5] * list[7]) -
           list[1] * (list[3] * list[8] - list[5] * list[6]) +
           list[2] * (list[3] * list[7] - list[4] * list[6]);
}

void inverse3(const float list[], float result[]) {
    float det = det3(list);
    if (det == 0.f) {
        return;
    }

    float inv_det = 1.f / det;

    result[0] = inv_det * (list[4] * list[8] - list[5] * list[7]);
    result[1] = inv_det * (list[2] * list[7] - list[1] * list[8]);
    result[2] = inv_det * (list[1] * list[5] - list[2] * list[4]);

    result[3] = inv_det * (list[5] * list[6] - list[3] * list[8]);
    result[4] = inv_det * (list[0] * list[8] - list[2] * list[6]);
    result[5] = inv_det * (list[2] * list[3] - list[0] * list[5]);

    result[6] = inv_det * (list[3] * list[7] - list[4] * list[6]);
    result[7] = inv_det * (list[1] * list[6] - list[0] * list[7]);
    result[8] = inv_det * (list[0] * list[4] - list[1] * list[3]);
}

void scalar_multiply(const float list[], float scalar, float result[], int n) {
    for (int i = 0; i < n; ++i) {
        result[i] = list[i] * scalar;
    }
}

float dot(const float list1[], const float list2[], int n) {
    float sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += list1[i] * list2[i];
    }
    
    return sum;
}

float change_of_base(const float list1[], const float list2[], int n) {
    float result = 0.f;
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result += list1[i] * list2[i * n + j] * list1[j];
        }
    }

    return result; 
}

void matrix_multiply(const float list1[], const float list2[], float result[], int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                result[i * n + j] += list1[i * n + k] * list2[k * n + j];
            }
        }
    }
}

void matrix_vector_multiply(const float list1[], const float list2[], float result[], int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i] += list1[i * n + j] * list2[j];
        }
    }
}

