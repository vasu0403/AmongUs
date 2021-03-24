#include "sprite_renderer.h"

SpriteRenderer::SpriteRenderer(const Shader &shader) {
    this->shader = shader;
    initRenderData();
}

void SpriteRenderer::initRenderData() {
    float vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    unsigned int VBO;
    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
    GL_STATIC_DRAW);
    glBindVertexArray(spriteVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->spriteVAO);
}

void SpriteRenderer::drawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
    // prepare transformations
    shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5*size.x, 0.5*size.y, 0.0));
    model = glm::rotate(model, glm::radians(rotate),
    glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(-0.5*size.x, -0.5*size.y, 0.0));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    shader.SetMatrix4("model", model);
    shader.SetVector3f("spriteColor", color);
    glActiveTexture(GL_TEXTURE0);
    texture.Bind();
    glBindVertexArray(spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}