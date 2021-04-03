#ifndef HUD_H
#define HUD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ft2build.h"
#include "bits/stdc++.h"
#include "shader.h"
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

class Hud {
    public:
    unsigned int VAO, VBO;
    Shader TextShader;
    Hud(Shader &textShader);
    void Display(int LivesLeft, bool PowerUpTask, bool EnemyTask, bool Light, int TimeLeft, int score);
    void GameLost(int score);
    void GameWon(int Score);
    private:
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
};
#endif