#ifndef GAME_OBJECT
#define GAME_OBJECT

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"


class GameObject {
public:
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    Texture2D   Sprite;	
    bool Destroyed;
    int Score;
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    void Draw(SpriteRenderer &renderer);
    void DrawSheet(SpriteRenderer &renderer, float start, float end, bool vertical = false);
};
#endif