#ifndef TRIANGLE_H
#define TRIANGLE_H
#pragma once
#include "vector.h"

typedef struct triangle_t
{
    vec2_t points[3];
} triangle_t;

typedef struct face_t
{
    int a, b, c;
} face_t;

#endif