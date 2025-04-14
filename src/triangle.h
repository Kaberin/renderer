#ifndef TRIANGLE_H
#define TRIANGLE_H
#pragma once
#include "vector.h"
#include "stdint.h"
typedef struct triangle_t
{
    vec2_t points[3];
    float avg_depth;
    uint32_t color;
} triangle_t;

typedef struct face_t
{
    int a, b, c;
    uint32_t color;
} face_t;

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif