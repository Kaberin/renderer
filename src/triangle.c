#include "triangle.h"

#include "display.h"
#include "swap.h"

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float inv_slope_1 = (float)(x1 - x0) / (float)(y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (float)(y2 - y0);

    float x_start = x0;
    float x_end = x0;

    for (int y = y0; y <= y1; y++)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float inv_slope_1 = (x2 - x0) / (float)(y2 - y0);
    float inv_slope_2 = (x2 - x1) / (float)(y2 - y1);

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y1; y--)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2)
    {
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
        return;
    }
    else if (y0 == y1)
    {
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
        return;
    }
    else
    {
        int My = y1;
        float denominator = y2 - y0;
        if (denominator == 0)
            return;
        int Mx = ((float)(y1 - y0) * (x2 - x0) / denominator) + x0;
        // draw_line(Mx, My, x1, y1, color);

        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
        return;
    }
    // fill_flat_top_triangle();
};

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);
    float area_abc = (ac.x * ab.y - ac.y * ab.x);
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_abc;
    float beta = (ac.x * ap.y - ap.x * ac.y) / area_abc;
    float gamma = 1.0 - alpha - beta;
    vec3_t weights = {.x = alpha, .y = beta, .z = gamma};
    return weights;
}

void draw_texel(int x, int y, uint32_t* texture, vec2_t point_a, vec2_t point_b, vec2_t point_c, float u0, float v0,
                float u1, float v1, float u2, float v2)
{
    vec2_t point_p = {x, y};
    vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    float interpolated_u = u0 * alpha + u1 * beta + u2 * gamma;
    float interpolated_v = v0 * alpha + v1 * beta + v2 * gamma;
    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));
    if ((tex_x < texture_width) && (tex_y < texture_height) && (tex_x >= 0) && (tex_y >= 0))
    {
        draw_pixel(x, y, texture[tex_y * texture_width + tex_x]);
    }
}
////////////////////////////////////
// Render bottom part of triangle //
////////////////////////////////////
void draw_textured_flat_top_triangle(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2,
                                     int y2, float u2, float v2, uint32_t* texture, vec2_t point_a, vec2_t point_b,
                                     vec2_t point_c)
{
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if ((y1 - y0) != 0)
    {
        inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    }
    if ((y2 - y0) != 0)
    {
        inv_slope_2 = (float)(x2 - x0) / (y2 - y0);
    }
    float x_start = x0;
    float x_end = x0;
    for (int y = y0; y <= y1; y++)
    {
        int x_s = (int)x_start;
        int x_e = (int)x_end;

        if (x_s > x_e)
        {
            int_swap(&x_s, &x_e);
        }
        for (int x = x_s; x < x_e; x++)
        {
            draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
        }
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}
///////////////////////////////////
// Render upper part of triangle //
///////////////////////////////////
void draw_textured_flat_bottom_triangle(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2,
                                        int y2, float u2, float v2, uint32_t* texture, vec2_t point_a, vec2_t point_b,
                                        vec2_t point_c)
{
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if ((y2 - y1) != 0)
    {
        inv_slope_1 = (float)(x2 - x1) / (y2 - y1);
    }
    if ((y2 - y0) != 0)
    {
        inv_slope_2 = (float)(x2 - x0) / (y2 - y0);
    }

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y1; y--)
    {
        int x_s = (int)x_start;
        int x_e = (int)x_end;

        if (x_e < x_s)
        {
            int_swap(&x_s, &x_e);
        }
        for (int x = x_s; x <= x_e; x++)
        {
            draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
        }
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}

void draw_textured_triangle(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2,
                            float u2, float v2, uint32_t* texture)
{
    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    vec2_t point_a = {x0, y0};
    vec2_t point_b = {x1, y1};
    vec2_t point_c = {x2, y2};

    if (y0 == y1)
    {
        draw_textured_flat_bottom_triangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, texture, point_a, point_b,
                                           point_c);
        return;
    }
    if (y1 == y2)
    {
        draw_textured_flat_top_triangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, texture, point_a, point_b,
                                        point_c);
        return;
    }

    draw_textured_flat_bottom_triangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, texture, point_a, point_b,
                                       point_c);

    draw_textured_flat_top_triangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, texture, point_a, point_b, point_c);
};