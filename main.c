

#include "material.h"
#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>

void drawMaterial(struct App* app, Material* mat, MaterialUniform** uniforms, int count);

int main() {
    // ----------------------
    // Initialize App
    // ----------------------
    App app = newApp("OpenGLES Offscreen Renderer", 800, 600);



    // ----------------------
    // Create Material
    // ----------------------
    Material* mat = newMaterial("example","shaders/vp.vert", "shaders/fp.frag");
    TextureOptions opts = {1, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};

    MaterialUniform* uTime = newMaterialUniform("uTime", UT_FLOAT, &(float){0.0f}, 0, NULL);
    MaterialUniform* uColor = newMaterialUniform("uColor", UT_VEC4, (float[]){1,1,1,1}, 0, NULL);
    MaterialUniform* uTex = newMaterialUniform("uTex", UT_SAMPLER2D, "shaders/image.jpg", 0, &opts);

    MaterialUniform* uniforms[] = {uTime, uColor, uTex};

    // ----------------------
    // Animation Example
    // ----------------------
    const int frames = 60;
    for(int i = 0; i < frames; i++) {
        *(float*)uTime->value = i * 0.1f;

        // Draw full-screen quad with material
        drawMaterial(&app, mat, uniforms, 3);

        // Save each frame
        char filename[64];
        sprintf(filename, "output/frame_%03d.png", i);
        saveView(&app, filename);
    }

    // ----------------------
    // Cleanup
    // ----------------------
    destroyMaterialUniform(uTime);
    destroyMaterialUniform(uColor);
    destroyMaterialUniform(uTex);
    destroyMaterial(mat);
    destroyApp(&app);

    return 0;
}

void drawMaterial(App* app, Material* mat, MaterialUniform** uniforms, int count) {
    enableMaterial(mat);
    for(int i=0;i<count;i++) setMaterialUniform(mat, uniforms[i]);
    glBindVertexArray(app->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    disableMaterial();
}