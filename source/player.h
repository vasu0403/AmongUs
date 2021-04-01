#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "bits/stdc++.h"
#include <GLFW/glfw3.h>


class PlayerObject : public GameObject {
    public:
    float Lives;
    int Score;
    int Ticks;
    int FrameIndex = 0;
    int FrameSpeed;
    bool Animation;
    float LastWalkTime;
    PlayerObject();
    PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // std::pair<int, int> GoToDirection(const std::vector<std::vector<bool>> &Allowed, int N, int M, std::pair<int, int> PlayerIndex, std::pair<int, int> EnemyIndex);
    void Draw(SpriteRenderer &renderer);
};
#endif