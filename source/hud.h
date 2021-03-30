#ifndef HUD_H
#define HUD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ft2build.h"
#include "bits/stdc++.h"
#include "shader.h"
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class Hud {
    public:
    unsigned int VAO, VBO;
    Shader TextShader;
    Hud(Shader &textShader);
    void Display(int LivesLeft, bool PowerUpTask, bool EnemyTask, bool Light, int TimeLeft, int score);
    private:
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
};
#endif