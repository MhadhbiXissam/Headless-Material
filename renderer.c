

#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

// Full-screen quad
static void createFullScreenQuad(App* app) {
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
    };
    glGenVertexArrays(1, &app->VAO);
    glGenBuffers(1, &app->VBO);

    glBindVertexArray(app->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

App newApp(const char* title, int width, int height) {
    App app;
    app.title = title;
    app.width = width;
    app.height = height;

    app.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (app.display == EGL_NO_DISPLAY) { printf("Failed to get EGL display\n"); exit(1); }
    if (!eglInitialize(app.display, NULL, NULL)) { printf("Failed to initialize EGL\n"); exit(1); }

    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(app.display, configAttribs, &config, 1, &numConfigs)) { printf("Failed to choose EGL config\n"); exit(1); }

    EGLint pbufferAttribs[] = { EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE };
    app.surface = eglCreatePbufferSurface(app.display, config, pbufferAttribs);
    if (!app.surface) { printf("Failed to create Pbuffer surface\n"); exit(1); }

    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    app.context = eglCreateContext(app.display, config, EGL_NO_CONTEXT, contextAttribs);
    if (!app.context) { printf("Failed to create EGL context\n"); exit(1); }
    if (!eglMakeCurrent(app.display, app.surface, app.surface, app.context)) { printf("Failed to make context current\n"); exit(1); }

    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    createFullScreenQuad(&app);
    return app;
}

void destroyApp(App* app) {
    glDeleteVertexArrays(1, &app->VAO);
    glDeleteBuffers(1, &app->VBO);
    eglDestroyContext(app->display, app->context);
    eglDestroySurface(app->display, app->surface);
    eglTerminate(app->display);
}

void swapAppBuffers(App* app) {
    eglSwapBuffers(app->display, app->surface);
}

void saveView(App* app, const char* filename) {
    unsigned char *pixels = malloc(app->width * app->height * 4);
    glReadPixels(0, 0, app->width, app->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Flip vertically
    for(int y=0;y<app->height/2;y++)
        for(int x=0;x<app->width*4;x++){
            unsigned char tmp = pixels[y*app->width*4+x];
            pixels[y*app->width*4+x] = pixels[(app->height-1-y)*app->width*4+x];
            pixels[(app->height-1-y)*app->width*4+x] = tmp;
        }

    if (!stbi_write_png(filename, app->width, app->height, 4, pixels, app->width*4))
        printf("Failed to save image\n");
    else
        printf("Saved %s\n", filename);

    free(pixels);
}



