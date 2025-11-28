#ifndef MATERIAL_H
#define MATERIAL_H
#include <GLES3/gl3.h>

typedef enum { UT_FLOAT, UT_VEC4, UT_SAMPLER2D } UniformType;

typedef struct {
    int flipVertically;
    GLenum minFilter;
    GLenum magFilter;
    GLenum wrapS;
    GLenum wrapT;
} TextureOptions;

typedef struct {
    char* name;
    UniformType type;
    void* value;
    GLuint textureUnit;
} MaterialUniform;

typedef struct {
    const char* name;
    GLuint program;
} Material;

Material* newMaterial(const char* name, const char* vertPath, const char* fragPath);
void destroyMaterial(Material* mat);

MaterialUniform* newMaterialUniform(const char* name, UniformType type, void* initVal, GLuint texUnit, TextureOptions* opts);
void destroyMaterialUniform(MaterialUniform* u);

void enableMaterial(Material* mat);
void disableMaterial();
void setMaterialUniform(Material* mat, MaterialUniform* u);
#endif // MATERIAL_H