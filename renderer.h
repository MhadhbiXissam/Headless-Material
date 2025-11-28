#pragma once
#include <EGL/egl.h>
#include <GLES3/gl3.h>

// =======================
// App & Full-screen Quad
// =======================
typedef struct App {
    const char* title;
    int width;
    int height;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    GLuint VAO;
    GLuint VBO;
} App;

App newApp(const char* title, int width, int height);
void destroyApp(App* app);
void swapAppBuffers(App* app);
void saveView(App* app, const char* filename);
