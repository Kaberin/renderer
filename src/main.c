#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#define M_PI 3.14159265359
bool is_running = false;
int previous_frame_time = 0;

vec3_t camera_poisition = {0, 0, 0};

float fov_factor = 640;
float rotation = 0;

triangle_t *triangles_to_render = NULL;

CullMethod cull_method = CULL_BACKFACE;
RenderMethod render_method = RENDER_WIRE;
mat4_t proj_matrix;

void setup(void)
{
    printf("Setting up...\n");
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    float fov = M_PI / 3;
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.08;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    mesh_texture = (uint32_t *)REDBRICK_TEXTURE;
    texture_width = 64;
    texture_height = 64;

    load_cube_mesh_data();
    // load_obj_file_data("/home/kaberin/Programming/SDL_course/assets/cube.obj");
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
            else if (key == SDLK_5)
            {
                render_method = RENDER_TEXTURED;
            }
            else if (key == SDLK_6)
            {
                render_method = RENDER_TEXTURED_WIRE;
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

// vec2_t project(vec3_t point)
// {
//     vec2_t projected = {.x = point.x * fov_factor / point.z, .y = point.y * fov_factor / point.z};
//     return projected;
// }

// ⁣⁣‍Своя реализация quicksort, но решил использовать qsort из стандартной библиотеки⁡
void sort_by_depth(triangle_t *triangles)
{
    int size = array_length(triangles);
    if (size == 0 || size == 1)
    {
        return;
    }
    int midpoint = size / 2;
    triangle_t mid_element = triangles[midpoint];
    triangle_t *less = NULL;
    triangle_t *equal = NULL;
    triangle_t *more = NULL;
    for (int i = 0; i < size; i++)
    {
        if (triangles[i].avg_depth < mid_element.avg_depth)
        {
            array_push(less, triangles[i]);
        }
        else if (triangles[i].avg_depth > mid_element.avg_depth)
        {
            array_push(more, triangles[i]);
        }
        else
        {
            array_push(equal, triangles[i]);
        }
    }
    sort_by_depth(less);
    sort_by_depth(more);

    int less_size = array_length(less);
    int equal_size = array_length(equal);
    int more_size = array_length(more);

    int index = 0;
    for (int i = 0; i < more_size; i++)
    {
        triangles[index++] = more[i];
    }
    for (int i = 0; i < equal_size; i++)
    {
        triangles[index++] = equal[i];
    }
    for (int i = 0; i < less_size; i++)
    {
        triangles[index++] = less[i];
    }
}

/// @brief Компаратор для triangle_t.avg_depth
/// @param a Забей
/// @param b Забей
/// @return
int depth_compare(const void *a, const void *b)
{
    float d1 = ((triangle_t *)a)->avg_depth;
    float d2 = ((triangle_t *)b)->avg_depth;
    if (d1 > d2)
    {
        return -1;
    }
    if (d1 < d2)
    {
        return 1;
    }
    return 0;
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
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;

    // mesh.rotation.x = M_PI;
    // mesh.translation.y = 1;
    mesh.translation.z = 5;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t rotation_x_matrix = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_y_matrix = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_z_matrix = mat4_make_rotation_z(mesh.rotation.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    // loop all faces of mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; ++i)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertcies[3];
        face_vertcies[0] = mesh.vertices[mesh_face.a - 1];
        face_vertcies[1] = mesh.vertices[mesh_face.b - 1];
        face_vertcies[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];
        // Loop all vertices and apply transformations
        for (int j = 0; j < 3; ++j)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertcies[j]);

            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_x_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_y_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_z_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
            transformed_vertices[j] = transformed_vertex;
        }
        // Calculating normal for current face
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);
        // Backface culling
        if (cull_method == CULL_BACKFACE)
        {
            vec3_t camera_ray = vec3_sub(camera_poisition, vector_a);
            float dot_normal_camera = vec3_dot(normal, camera_ray);
            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        // printf("Degree is %f\n", degree);
        // getchar();
        // if (light_intensity_factor < 0)
        // {
        //     if (light_intensity_factor < 0.5)
        //     {
        //         mesh_face.color = light_apply_intensity(mesh_face.color, fabs(light_intensity_factor));
        //     }
        // }
        // else if (light_intensity_factor == 1)
        // {
        //     mesh_face.color = 0xFF000000;
        // }
        // else
        // {
        // }

        vec4_t projected_points[3];

        for (int j = 0; j < 3; ++j)
        {
            // projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));
            projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

            // NDC to Viewport transform
            // Scale into view
            projected_points[j].x *= (window_width / 2);
            projected_points[j].y *= (window_height / 2);

            // invert y-values
            projected_points[j].y *= -1;

            // Translate to middle of screen
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }

        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

        // Flat shading

        // vec3_normalize(&light.direction);
        float light_intensity_factor = -vec3_dot(light.direction, normal);
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = {
            .points =
                {
                    {projected_points[0].x, projected_points[0].y},
                    {projected_points[1].x, projected_points[1].y},
                    {projected_points[2].x, projected_points[2].y},
                },
            .color = triangle_color,
            .avg_depth = avg_depth,
            .texcoords = {
                {mesh_face.a_uv.u, mesh_face.a_uv.v},
                {mesh_face.b_uv.u, mesh_face.b_uv.v},
                {mesh_face.c_uv.u, mesh_face.c_uv.v},
            },
        };

        array_push(triangles_to_render, projected_triangle);
    }

    // sort triangles_to_render by avg_depth (painter algorythm)
    // sort_by_depth(triangles_to_render);
    qsort(triangles_to_render, array_length(triangles_to_render), sizeof(triangle_t), depth_compare);
}

void render(void)
{
    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
        {
            draw_textured_triangle(triangle.points[0].x, triangle.points[0].y, triangle.texcoords[0].u, triangle.texcoords[0].v,
                                   triangle.points[1].x, triangle.points[1].y, triangle.texcoords[1].u, triangle.texcoords[1].v,
                                   triangle.points[2].x, triangle.points[2].y, triangle.texcoords[2].u, triangle.texcoords[2].v,
                                   mesh_texture);
        }

        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, triangle.color);
        }
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE)
        {
            draw_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFFFF);
        }
        if (render_method == RENDER_WIRE_VERTEX)
        {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 4, 4, 0xFF00FF00);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 4, 4, 0xFF00FF00);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 4, 4, 0xFF00FF00);
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