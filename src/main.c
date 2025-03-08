#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
bool is_running = false;
int previous_frame_time = 0;

vec3_t camera_poisition = {0, 0, -5};

vec3_t cube_rotation = {.x = 0, .z = 0, .y = 0};

float fov_factor = 640;

float rotation = 0;

triangle_t triangles_to_render[N_MESH_FACES];

void setup(void)
{
    printf("Setting up...\n");
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
}

void process_input(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
            }
            break;
        default:
            break;
        }
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected = {.x = point.x * fov_factor / point.z, .y = point.y * fov_factor / point.z};
    return projected;
}

void update(void)
{

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks64() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks64();

    cube_rotation.y += 0.01;
    cube_rotation.x += 0.02;
    cube_rotation.x += 0.005;

    // loop all faces of mesh
    for (int i = 0; i < N_MESH_FACES; ++i)
    {
        face_t mesh_face = mesh_faces[i];

        vec3_t face_vertcies[3];
        face_vertcies[0] = mesh_vertices[mesh_face.a - 1];
        face_vertcies[1] = mesh_vertices[mesh_face.b - 1];
        face_vertcies[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        // Loop all vertices and apply transformations
        for (int j = 0; j < 3; ++j)
        {
            vec3_t transformed_vertex = face_vertcies[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            transformed_vertex.z -= camera_poisition.z;

            vec2_t projected_point = project(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }

        triangles_to_render[i] = projected_triangle;
    }
}

void render(void)
{
    draw_grid(0xFF333333);

    for (int i = 0; i < N_MESH_FACES; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
    }

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

int main(void)
{
    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();
        update();
        render();
    }
    destroy_window();
    printf("Renderer finished work!\n");
    return 0;
}