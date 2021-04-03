#ifndef SPRITE
#define SPRITE

#include "shader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
class SpriteRenderer {
    private:
    Shader shader;
    void initRenderData();
    unsigned int spriteVAO;

    public:
    SpriteRenderer(const Shader &shader);
    ~SpriteRenderer();
    void drawSprite(const Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void drawSpriteSheet(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, float start, float end, glm::vec3 color = glm::vec3(1.0f));
    void drawSpriteSheetVertical(const Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate, float start, float end, glm::vec3 color = glm::vec3(1.0f));
};

#endif