#include "display.h"
/*
Colors in hexadecimal
  A R G B
0xFFFF0000
*/

int window_width = 800;
int window_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;
uint32_t *color_buffer = NULL;

bool initialize_window(void)
{
    printf("Initializing window...\n");
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        fprintf(stderr, "Failed to initialize SDL!\n");
        return false;
    };

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        fprintf(stderr, "Failed to create window!\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Failed to create renderer!\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void destroy_window(void)
{
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
}

void draw_pixel(int x, int y, uint32_t color)
{
    if (x >= 0 && x < window_width && y >= 0 && y < window_height)
    {
        color_buffer[(y * window_width) + x] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color)
{
    for (int _y = y; _y < y + height; _y++)
    {
        for (int _x = x; _x < x + width; _x++)
        {
            draw_pixel(_x, _y, color);
        }
    }
}

void draw_grid(uint32_t color)
{
    for (int y = 0; y < window_height; y += 10)
    {
        for (int x = 0; x < window_width; x += 10)
        {
            draw_pixel(x, y, color);
        }
    }
}

void clear_color_buffer(uint32_t color)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            color_buffer[(y * window_width) + x] = color;
        }
    }
}

void render_color_buffer(void)
{
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(sizeof(uint32_t) * window_width));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}