// renderer.c
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "material.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// ------------------------
// File Reading
// ------------------------
static char* read_file(const char* filename) {
    FILE *file = fopen(filename, "rb");
    if(!file){ perror("Failed to open file"); return NULL; }
    fseek(file,0,SEEK_END);
    long size = ftell(file);
    rewind(file);
    char *buffer = malloc(size+1);
    if(!buffer){ perror("Failed to allocate buffer"); fclose(file); return NULL; }
    fread(buffer,1,size,file);
    buffer[size]='\0';
    fclose(file);
    return buffer;
}

// ------------------------
// Shader Compilation
// ------------------------
static GLuint compile_shader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader,1,&src,NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success){
        char info[512];
        glGetShaderInfoLog(shader,512,NULL,info);
        printf("Shader compile error (%s): %s\n",type==GL_VERTEX_SHADER?"VERTEX":"FRAGMENT",info);
    }
    return shader;
}

static GLuint create_program(const char* vertSrc, const char* fragSrc){
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program,GL_LINK_STATUS,&success);
    if(!success){
        char info[512];
        glGetProgramInfoLog(program,512,NULL,info);
        printf("Program link error: %s\n",info);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

// ------------------------
// Texture Loader
// ------------------------
static GLuint load_texture_uniform_opts(const char* filename, GLuint unit, TextureOptions opts){
    stbi_set_flip_vertically_on_load(opts.flipVertically);
    int w,h,ch;
    unsigned char* data = stbi_load(filename,&w,&h,&ch,0);
    if(!data){ fprintf(stderr,"Failed to load texture: %s\n",filename); exit(1); }

    GLuint tex;
    glGenTextures(1,&tex);
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_2D,tex);
    GLenum format = (ch==4)?GL_RGBA:GL_RGB;
    glTexImage2D(GL_TEXTURE_2D,0,format,w,h,0,format,GL_UNSIGNED_BYTE,data);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,opts.minFilter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,opts.magFilter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,opts.wrapS);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,opts.wrapT);

    stbi_image_free(data);
    return tex;
}

// ------------------------
// Material Functions
// ------------------------
Material* newMaterial(const char* name, const char* vertPath, const char* fragPath){
    char* vertSrc = read_file(vertPath);
    char* fragSrc = read_file(fragPath);
    if(!vertSrc || !fragSrc) return NULL;

    GLuint program = create_program(vertSrc,fragSrc);
    free(vertSrc); free(fragSrc);

    Material* mat = malloc(sizeof(Material));
    mat->name = strdup(name);
    mat->program = program;
    return mat;
}

void destroyMaterial(Material* mat){
    if(!mat) return;
    free((char*)mat->name);
    glDeleteProgram(mat->program);
    free(mat);
}

MaterialUniform* newMaterialUniform(const char* name, UniformType type, void* initVal, GLuint texUnit, TextureOptions* opts){
    MaterialUniform* u = malloc(sizeof(MaterialUniform));
    u->name = strdup(name);
    u->type = type;
    u->textureUnit = texUnit;
    u->value = NULL;

    if(type==UT_FLOAT){
        u->value = malloc(sizeof(float));
        *(float*)u->value = initVal ? *(float*)initVal : 0.0f;
    } else if(type==UT_VEC4){
        u->value = malloc(sizeof(float)*4);
        if(initVal) memcpy(u->value, initVal, sizeof(float)*4);
    } else if(type==UT_SAMPLER2D && initVal && opts){
        u->value = malloc(sizeof(GLuint));
        *(GLuint*)u->value = load_texture_uniform_opts((const char*)initVal, texUnit, *opts);
    }
    return u;
}

void destroyMaterialUniform(MaterialUniform* u){
    if(!u) return;
    free(u->name);
    if(u->type==UT_SAMPLER2D && u->value) glDeleteTextures(1,(GLuint*)u->value);
    free(u->value);
    free(u);
}

void enableMaterial(Material* mat){ glUseProgram(mat->program); }
void disableMaterial(){ glUseProgram(0); }

void setMaterialUniform(Material* mat, MaterialUniform* u){
    if(!mat||!u) return;
    GLint loc = glGetUniformLocation(mat->program,u->name);
    if(loc<0) return;
    switch(u->type){
        case UT_FLOAT: glUniform1f(loc,*(float*)u->value); break;
        case UT_VEC4: glUniform4fv(loc,1,(float*)u->value); break;
        case UT_SAMPLER2D:
            glActiveTexture(GL_TEXTURE0+u->textureUnit);
            glBindTexture(GL_TEXTURE_2D,*(GLuint*)u->value);
            glUniform1i(loc,u->textureUnit);
            break;
    }
}

