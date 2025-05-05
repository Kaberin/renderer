#ifndef DISPLAY_H
#define DISPLAY_H
#pragma once
#include "triangle.h"
#include "SDL2/SDL.h"
#include "stdint.h"
#include "stdbool.h"

#define FPS 60
#define FRAME_TARGET_TIME 1000 / FPS

typedef enum
{
    CULL_NONE,
    CULL_BACKFACE
} CullMethod;

typedef enum
{
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
} RenderMethod;

extern int window_width;
extern int window_height;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern uint32_t *color_buffer;

bool initialize_window(void);
void destroy_window(void);

void draw_pixel(int x, int y, uint32_t color);
void draw_grid(uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
// void draw_line_dda(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);

#endif