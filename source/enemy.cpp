#include "enemy.h"
using namespace std;
EnemyObject::EnemyObject(): GameObject(), Alive(true), GoToCell({-1, -1}), LastAttack(-100) {
}
EnemyObject::EnemyObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, pair<int, int> goToCell, const vector<vector<bool>> &Allowed, int N, int M, glm::vec3 color, glm::vec2 velocity): GameObject(pos, size, sprite, color, velocity), Alive(true), GoToCell(goToCell), LastAttack(-100) {
    for(int X = 0; X < N; X++) {
        for(int Y = 0; Y < M; Y++) {
            if(Allowed[X][Y]) {
                FillGoToCells(X, Y, N, M, Allowed);
            }
        }
    }
    cout << "Finished " << "\n";
}

void EnemyObject::FillGoToCells(int X, int Y, int N, int M, const vector<vector<bool>> &Allowed) {
    vector<vector<bool>> Vis(N, vector<bool> (M, false));
    Vis[X][Y] = true;
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    // bfs :)
    queue<pair<int, int>> q;
    this->GoTo[X][Y][X][Y] = {X, Y};
    q.push({X, Y});
    while(!q.empty()) {
        pair<int, int> Cur = q.front();
        q.pop();
        int X1 = Cur.first;
        int Y1 = Cur.second;
        for(int i = 0; i < 4; i++) {
            int X2 = X1 + dx[i];
            int Y2 = Y1 + dy[i];
            if(Valid(X2, Y2, Allowed, N, M) && !Vis[X2][Y2]) {
                pair<int, int> Adjacent = make_pair(X2, Y2);
                this->GoTo[X][Y][X2][Y2] = {X1, Y1};
                q.push(Adjacent);
                Vis[X2][Y2] = true;
            }
        }
    }
}
void EnemyObject::Draw(SpriteRenderer &renderer) {
    renderer.drawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

bool EnemyObject::Valid(int X, int Y, const std::vector<std::vector<bool>> &Allowed, int N, int M) {
    return X >= 0 && X < N && Y >= 0 && Y < M && Allowed[X][Y]; 
}