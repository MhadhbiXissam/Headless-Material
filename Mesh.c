#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------
// Mesh system
// ------------------------------------------------------
typedef struct {
    char     name[32];
    GLint    components;
    GLenum   type;
    GLsizei  type_size;
    void*    data;
    int      vertex_count;
} MeshAttribute;

typedef struct {
    MeshAttribute* attrs;
    int count;
} MeshBuilder;

typedef struct {
    GLuint vao;
    GLuint vbo;
    int vertex_count;
    int stride;
} Mesh;

static inline GLsizei glTypeSize(GLenum type) {
    switch (type) {
        case GL_FLOAT: return 4;
        case GL_UNSIGNED_BYTE: return 1;
        case GL_UNSIGNED_SHORT: return 2;
        default: return 4;
    }
}

MeshBuilder MeshBuilder_Create() {
    MeshBuilder b;
    b.attrs = NULL;
    b.count = 0;
    return b;
}

void MeshBuilder_AddAttribute(MeshBuilder* b, const char* name, GLint components, GLenum type, void* data, int vertex_count) {
    b->attrs = realloc(b->attrs, sizeof(MeshAttribute)*(b->count+1));
    MeshAttribute* a = &b->attrs[b->count++];
    snprintf(a->name, 31, "%s", name);
    a->components = components;
    a->type = type;
    a->type_size = glTypeSize(type);
    a->data = data;
    a->vertex_count = vertex_count;
}

Mesh MeshBuilder_Build(MeshBuilder* b) {
    Mesh mesh;
    memset(&mesh, 0, sizeof(Mesh));
    if (b->count == 0) return mesh;

    int vertex_count = b->attrs[0].vertex_count;
    int stride = 0;
    for (int i=0; i < b->count; i++)
        stride += b->attrs[i].components * b->attrs[i].type_size;

    mesh.vertex_count = vertex_count;
    mesh.stride = stride;

    unsigned char* buffer = malloc(vertex_count * stride);
    for (int v=0; v < vertex_count; v++) {
        int offset = 0;
        for (int i=0; i < b->count; i++) {
            MeshAttribute* a = &b->attrs[i];
            int size = a->components * a->type_size;
            memcpy(buffer + v*stride + offset, (unsigned char*)a->data + v*size, size);
            offset += size;
        }
    }

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * stride, buffer, GL_STATIC_DRAW);

    free(buffer);

    int offset = 0;
    int attrib = 0;
    for (int i=0; i < b->count; i++) {
        MeshAttribute* a = &b->attrs[i];
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(attrib, a->components, a->type, GL_FALSE, stride, (void*)(long)offset);
        offset += a->components * a->type_size;
        attrib++;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return mesh;
}

void Mesh_Draw(Mesh* m) {
    glBindVertexArray(m->vao);
    glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
    glBindVertexArray(0);
}


    // // --------------------------------------------------
    // // Triangle mesh
    // // --------------------------------------------------
    // float positions[] = {
    //      0.0f,  0.8f, 0.0f,
    //     -0.8f, -0.8f, 0.0f,
    //      0.8f, -0.8f, 0.0f
    // };
    // float uvs[] = {
    //     0.5f, 1.0f,
    //     0.0f, 0.0f,
    //     1.0f, 0.0f
    // };

    // MeshBuilder builder = MeshBuilder_Create();
    // MeshBuilder_AddAttribute(&builder, "pos", 3, GL_FLOAT, positions, 3);
    // MeshBuilder_AddAttribute(&builder, "uv",  2, GL_FLOAT, uvs,       3);

    // Mesh mesh = MeshBuilder_Build(&builder);
    // Mesh_Draw(&mesh);
