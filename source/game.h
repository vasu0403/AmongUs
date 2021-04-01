#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "texture.h"
#include "game_object.h"
#include <bits/stdc++.h>
#include "hud.h"


const float AspectRatio = 542.0 / 480.0;   // Height / Width
const float Width = 30.0f;
// const float Width = 100.0f;
const glm::vec2 PLAYER_SIZE(Width, Width * AspectRatio);
const float PLAYER_VELOCITY(250.0f);
const float GapBetweenAttacks = 1;

const std::vector<std::pair<float, float>> PlayerFramePositions = {{0.0, 0.20}, {0.30, 0.50}, {0.57, 0.75}, {0.80, 1.0}};

const int CoinFrames = 6;
const float FrameSpeed = 0.1;
// 480/522
struct Level {
    std::vector<GameObject> Walls;
    std::pair<int, int> Exit;
    std::pair<int, int> ButtonEnemy;
    std::pair<int, int> ButtonPowerUps;
    std::vector<std::pair<float, float>> PlayArea;
    std::vector<std::vector<bool>> Vis;
};


class Game {
public:	
    bool Keys[1024];
    float Width, Height;
    float LeftBorder, RightBorder;
    float TopBorder, BottomBorder;
    float WallSize;
    float LightKeyUnpressed;
    float LightOn;
    std::vector<std::vector<bool>> IsWall(std::vector<bool>);
    glm::vec3 cameraUp;
    glm::vec3 cameraFront;
    glm::vec3 cameraPos;
    Shader SpriteShader;
    Shader TextShader;
    Hud* HUD;
    int TimeLeft;
    float LastTick;
    bool GameLost;
    bool GameWon;
    float LightScoreTimeLast;

    std::vector<GameObject> Walls;
    std::vector<std::vector<bool>> Vis;
    GameObject* EnemyButton;
    GameObject* PowerUpButton;
    bool EnemyButtonPressed;                            // whether enemy button bettun has been pressed (true) or not (false)
    bool PowerUpButtonPressed;                          // whether power button bettun has been pressed (true) or not (false)
    GameObject* ExitDoor;                             
    std::vector<std::pair<float, float>> PlayArea;      // tiles on which the player can move
    std::vector<GameObject> PowerUps;                   // coins and bomb objects
    
    Game(float width, float height, int timeLeft);
    void LoadShaders();
    void LoadTextures();
    void LoadFonts();
    void LoadLevel();
    void CameraInit();
    void Render();
    void ProcessInput(float dt);
    void Update(float dt);

    private:
    Level MakeMaze();
    bool valid(std::pair<int, int> P, int N, int M);
    bool CollisionWithWall();
    void UpdateEnemy(float dt);
    bool CollisionWithDoor();
};

#endif