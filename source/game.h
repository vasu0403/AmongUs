#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "texture.h"
#include "game_object.h"
#include <bits/stdc++.h>


const float AspectRatio = 542.0 / 480.0;   // Height / Width
const float Width = 30.0f;
const glm::vec2 PLAYER_SIZE(Width, Width * AspectRatio);
const float PLAYER_VELOCITY(500.0f);
// 480/522
class Game {
public:	
    bool Keys[1024];
    float Width, Height;
    float LeftBorder, RightBorder;
    float TopBorder, BottomBorder;
    float WallSize;
    glm::vec3 cameraUp;
    glm::vec3 cameraFront;
    glm::vec3 cameraPos;
    Shader SpriteShader;
    std::vector<GameObject> Walls;
    Game(float width, float height);
    void LoadShaders();
    void LoadTextures();
    void LoadLevel();
    void CameraInit();
    void Render();
    void ProcessInput(float dt);

    private:
    std::vector<GameObject> MakeMaze();
    bool valid(std::pair<int, int> P, int N, int M);
};

#endif