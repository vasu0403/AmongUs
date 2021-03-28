#ifndef ENEMY_H
#define ENEMY_H

#include "game_object.h"
#include "bits/stdc++.h"


class EnemyObject : public GameObject {
    public:
    std::pair<int, int> GoToCell;
    bool Alive;
    std::pair<int, int> GoTo[40][40][40][40];

    EnemyObject();
    EnemyObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, std::pair<int, int> goToCell, const std::vector<std::vector<bool>> &Allowed, int N, int M,
        glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // std::pair<int, int> GoToDirection(const std::vector<std::vector<bool>> &Allowed, int N, int M, std::pair<int, int> PlayerIndex, std::pair<int, int> EnemyIndex);
    void Draw(SpriteRenderer &renderer);

    private:
    bool Valid(int X, int Y, const std::vector<std::vector<bool>> &Allowed, int N, int M);
    void FillGoToCells(int X, int Y, int N, int M, const std::vector<std::vector<bool>> &Allowed);
};
#endif