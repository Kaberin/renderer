#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
bool is_running = false;
int previous_frame_time = 0;

vec3_t camera_poisition = {0, 0, 0};

float fov_factor = 640;
float rotation = 0;

triangle_t *triangles_to_render = NULL;

CullMethod cull_method = CULL_BACKFACE;
RenderMethod render_method = RENDER_WIRE;

void setup(void)
{
    printf("Setting up...\n");
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    // load_cube_mesh_data();

    load_obj_file_data2("/home/kaberin/Programming/SDL_course/assets/cube.obj");
    // load_bunny("/home/kaberin/Programming/SDL_course/assets/bunny.obj");
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
            SDL_KeyCode key = event.key.keysym.sym;
            if (key == SDLK_ESCAPE)
            {
                is_running = false;
            }
            else if (key == SDLK_DOWN)
            {
                mesh.rotation.y += 1;
            }
            else if (key == SDLK_UP)
            {
                mesh.rotation.x += 1;
            }
            else if (key == SDLK_1)
            {
                render_method = RENDER_WIRE_VERTEX;
            }
            else if (key == SDLK_2)
            {
                render_method = RENDER_WIRE;
            }
            else if (key == SDLK_3)
            {
                render_method = RENDER_FILL_TRIANGLE;
            }
            else if (key == SDLK_4)
            {
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            }
            else if (key == SDLK_c)
            {
                if (cull_method == CULL_BACKFACE)
                {
                    cull_method = CULL_NONE;
                }
                else
                {
                    cull_method = CULL_BACKFACE;
                }
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

    triangles_to_render = NULL;

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks64() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks64();

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.02;
    mesh.rotation.z += 0.007;

    // loop all faces of mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; ++i)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertcies[3];
        face_vertcies[0] = mesh.vertices[mesh_face.a - 1];
        face_vertcies[1] = mesh.vertices[mesh_face.b - 1];
        face_vertcies[2] = mesh.vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        vec3_t transformed_vertices[3];
        // Loop all vertices and apply transformations
        for (int j = 0; j < 3; ++j)
        {
            vec3_t transformed_vertex = face_vertcies[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z += 5;
            transformed_vertices[j] = transformed_vertex;
        }

        // Backface culling
        if (cull_method == CULL_BACKFACE)
        {
            vec3_t vector_a = transformed_vertices[0];
            vec3_t vector_b = transformed_vertices[1];
            vec3_t vector_c = transformed_vertices[2];
            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_t normal = vec3_cross(vector_ab, vector_ac);
            vec3_normalize(&normal);

            vec3_t camera_ray = vec3_sub(camera_poisition, vector_a);
            float dot_normal_camera = vec3_dot(normal, camera_ray);
            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        for (int j = 0; j < 3; ++j)
        {
            vec2_t projected_point = project(transformed_vertices[j]);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }

        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void)
{
    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFF666666);
        }
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFFFF);
        }
        if (render_method == RENDER_WIRE_VERTEX)
        {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFF00FF00);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFF00FF00);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFF00FF00);
        }
    }

    // draw_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);
    // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFF00FF00);

    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    array_free(mesh.faces);
    array_free(mesh.vertices);
    free(color_buffer);
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
    free_resources();
    printf("Renderer finished work!\n");
    return 0;
}