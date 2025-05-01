#include "array.h"
#include "mesh.h"
#include "string.h"
mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0},
    .scale = {1, 1, 1},
    .translation = {0, 0, 0},
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},
    {.x = -1, .y = 1, .z = -1},
    {.x = 1, .y = 1, .z = -1},
    {.x = 1, .y = -1, .z = -1},
    {.x = 1, .y = 1, .z = 1},
    {.x = 1, .y = -1, .z = 1},
    {.x = -1, .y = 1, .z = 1},
    {.x = -1, .y = -1, .z = 1},
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3},
    {.a = 1, .b = 3, .c = 4},
    // right
    {.a = 4, .b = 3, .c = 5},
    {.a = 4, .b = 5, .c = 6},
    // back
    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},
    // left
    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},
    // top
    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},
    // bottom
    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4},
};

void load_cube_mesh_data(void)
{
    for (int i = 0; i < N_CUBE_VERTICES; i++)
    {
        array_push(mesh.vertices, cube_vertices[i]);
    }
    for (int i = 0; i < N_CUBE_FACES; i++)
    {
        array_push(mesh.faces, cube_faces[i]);
    }
}
/*
Returns true if success
 */

void load_obj_file_data(char *filename)
{
    FILE *file;
    file = fopen(filename, "r");
    char line[1024];

    while (fgets(line, 1024, file))
    {
        if (strncmp(line, "v ", 2) == 0)
        {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        if (strncmp(line, "f ", 2) == 0)
        {

            face_t face;
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &vertex_indices[0],
                   &texture_indices[0],
                   &normal_indices[0],
                   &vertex_indices[1],
                   &texture_indices[1],
                   &normal_indices[1],
                   &vertex_indices[2],
                   &texture_indices[2],
                   &normal_indices[2]);

            face.a = vertex_indices[0];
            face.b = vertex_indices[1];
            face.c = vertex_indices[2];
            face.color = 0xFF808080; // ⁡⁢⁣⁢Ц͟В͟Е͟Т П͟О͟В͟Е͟Р͟Х͟Н͟О͟С͟Т͟И З͟А͟Х͟А͟Р͟Д͟К͟О͟Д͟И͟Л Т͟У͟Т⁡ !!!
            array_push(mesh.faces, face);
        }
    }

    fclose(file);
}

void load_bunny(char *filename)
{
    FILE *file = NULL;
    file = fopen(filename, "r");
    char line[1024];

    while (fgets(line, 1024, file))
    {
        if (strncmp(line, "v", 1) == 0)
        {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        if (strncmp(line, "f", 1) == 0)
        {
            face_t face;
            sscanf(line, "f %d %d %d", &face.a, &face.b, &face.c);
            array_push(mesh.faces, face);
        }
    }

    fclose(file);
}

// bool load_obj_file_data2(char *filename)
// {
//     printf("\n\nLoading .obj model... [%s]\n", filename);
//     FILE *fptr = NULL;
//     fptr = fopen(filename, "r");
//     if (fptr == NULL)
//     {
//         printf("Failed to open...\n");
//         return false;
//     }
//     printf("Opened successful...\n");
//     char c = '\0';
//     printf("Starting reading...\n");
//     while ((c = getc(fptr)) != EOF)
//     {

//         switch (c)
//         {
//         case 'v':
//         {
//             float v1 = 0, v2 = 0, v3 = 0;
//             fscanf(fptr, "%f %f %f", &v1, &v2, &v3);
//             vec3_t vertice = {.x = v1, .y = v2, .z = v3};
//             array_push(mesh.vertices, vertice);
//             break;
//         }
//         case 'f':
//         {
//             int i1 = 0, i2 = 0, i3 = 0;
//             int skip = 0;
//             fscanf(fptr, "%d/%d/%d %d/%d/%d %d/%d/%d", &i1, &skip, &skip, &i2, &skip, &skip, &i3, &skip, &skip);
//             face_t face = {.a = i1, .b = i2, .c = i3};
//             array_push(mesh.faces, face);
//             break;
//         }
//         }
//     }

//     fclose(fptr);
//     return true;
// }
