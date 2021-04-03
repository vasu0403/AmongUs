#include "hud.h"
#include "bits/stdc++.h"
using namespace std;
std::map<GLchar, Character> Characters;
Hud::Hud(Shader& textShader) {
    this->TextShader = textShader;
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
    FT_Face face;
    if (FT_New_Face(ft, "../assets/fonts/Antonio-Regular.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Hud::Display(int livesLeft, bool powerUpTask, bool enemyTask, bool light, int timeLeft, int score) {
    string Lives = "Lives left: ";
    string LivesLeft_string = to_string(livesLeft);
    Lives += LivesLeft_string;
    
    string LightStatus = "Light: ";
    if(light) {
        LightStatus += "ON";
    } else {
        LightStatus += "OFF";
    }
    string Task = "Task: ";
    int Completed = 0;
    if(powerUpTask) {
        Completed += 1;
    }
    if(enemyTask) {
        Completed += 1;
    }
    string Completed_string = to_string(Completed);
    Task += Completed_string + " / 2";
    string Timer = "Time Left: ";
    string TimeLeft_string = to_string(timeLeft);
    Timer += TimeLeft_string;

    string Score = "Score: ";
    string score_string = to_string(score);
    Score += score_string;
    RenderText(Lives, 25.0f, 10.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));
    RenderText(Score, 135.0f, 10.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));
    RenderText(Timer, 215.0f, 10.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));
    RenderText(LightStatus, 330.0f, 10.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0));
    RenderText(Task, 420.0f, 10.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.0f));
}
void Hud::RenderText(std::string text, float x, float y, float scale, glm::vec3 color) {
    this->TextShader.Use();
    glUniform3f(glGetUniformLocation(this->TextShader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Hud::GameLost(int score) {
    RenderText("YOU LOST", 530.0f, 450.0f, 3.0f, glm::vec3(0.8f, 0.8f, 0.8f));
    string Score = "Score: ";
    string score_string = to_string(score);
    Score = Score + score_string;
    RenderText(Score, 690.0f, 390.0f, 0.8f, glm::vec3(0.8f, 0.8f, 0.8f));
}

void Hud::GameWon(int score) {
    RenderText("VICTORY !!", 530.0f, 450.0f, 3.0f, glm::vec3(0.8f, 0.8f, 0.8f));
    string Score = "Score: ";
    string score_string = to_string(score);
    Score = Score + score_string;
    RenderText(Score, 720.0f, 390.0f, 0.8f, glm::vec3(0.8f, 0.8f, 0.8f));
}