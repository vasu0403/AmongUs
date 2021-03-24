#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include <iostream>
using namespace std;
// Game-related State data
SpriteRenderer  *Renderer;
GameObject *Player;
Game::Game(float width, float height): Width(width), Height(height) { 
    ;
}

void Game::LoadShaders() {
    this->SpriteShader = ResourceManager::LoadShader("../source/sprite.vs", "../source/sprite.fs", nullptr, "sprite");
    this->SpriteShader.Use();
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    this->SpriteShader.SetInteger("image", 0);
    this->SpriteShader.SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(this->SpriteShader);

}
void Game::LoadTextures() {
    ResourceManager::LoadTexture("../assets/textures/player.png", true, "player");
    ResourceManager::LoadTexture("../assets/textures/background4.jpg", false, "background");
    ResourceManager::LoadTexture("../assets/textures/background.jpg", false, "wall");
    // this->cameraPos = glm::vec3(393.036f, 352.741f, 625.026f);
}
void Game::LoadLevel() {
    // glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    glm::vec2 playerPos = glm::vec2(0.0f, 0.0f);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
    this->WallSize = 50.0f;
    this->Walls = MakeMaze();
}

void Game::CameraInit() {
    this->cameraPos = glm::vec3(Player->Position.x, Player->Position.y  , -625.026f);
    this->cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    this->cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

    this->BottomBorder = 740.f;
    this->TopBorder = 255.0f;

    this->RightBorder = 350.0f;
    this->LeftBorder = 1213.0f;

}

void Game::Render() {
    cameraPos.y = min(cameraPos.y, this->BottomBorder);
    cameraPos.y = max(cameraPos.y, this->TopBorder);
    cameraPos.x = min(cameraPos.x, this->LeftBorder);
    cameraPos.x = max(cameraPos.x, this->RightBorder);

    glm::mat4 view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
    
    // view = glm::mat4(1.0f);
    this->SpriteShader.Use();
    this->SpriteShader.SetMatrix4("view", view);
    Renderer->drawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    
    for(GameObject Wall: this->Walls) {
        Wall.Draw(*Renderer);
    }
    
    Player->Draw(*Renderer);

}

void Game::ProcessInput(float dt) {
    glm::vec2 Direction1 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance1 = glm::length(Direction1);
    float velocity = PLAYER_VELOCITY * dt;
    if(this->Keys[GLFW_KEY_D]) {
        if(Player->Position.x <= this->Width - Player->Size.x) {
            Player->Position.x += velocity;
        } else {
            Player->Position.x = this->Width - Player->Size.x;
        }
    } else if(this->Keys[GLFW_KEY_A]) {
        if(Player->Position.x >= 0.0f) {
            Player->Position.x -= velocity;
        } else {
            Player->Position.x = 0.0f;
        }
    } else if(this->Keys[GLFW_KEY_W]) {
        if(Player-> Position.y >= 0.0f) {
            Player->Position.y -= velocity;
        } else {
            Player->Position.y = 0.0f;
        }
    } else if(this->Keys[GLFW_KEY_S]) {
        if(Player->Position.y <= this->Height - Player->Size.y) {
            Player->Position.y += velocity;
        } else {
            Player->Position.y = this->Height - Player->Size.y;
        }
    }
    glm::vec2 Direction2 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance2 = glm::length(Direction2);
    if(Distance2 > Distance1 && Distance2 >= 210) {
        if(this->Keys[GLFW_KEY_D]) {
            if(this->cameraPos.x < Player->Position.x) {
                this->cameraPos.x += velocity;
            }
        } else if(this->Keys[GLFW_KEY_A]) {
            if(this->cameraPos.x > Player->Position.x) {
                this->cameraPos.x -= velocity;
            }
        } else if(this->Keys[GLFW_KEY_W]) {
            if(this->cameraPos.y > Player->Position.y) {
                this->cameraPos.y -= velocity;
            }
        } else if(this->Keys[GLFW_KEY_S]) {
            if(this->cameraPos.y < Player->Position.y) {
                this->cameraPos.y += velocity;
            }
        }
    }
}

bool Valid(pair<int, int> P, int N, int M) {
    return P.first >= 0 && P.first < N && P.second >= 0 && P.second < M;
}
std::vector<GameObject> Game::MakeMaze() {
    int N = this->Height / this->WallSize;
    int M = this->Width / this->WallSize;
    vector<vector<bool>> Vis(N, vector<bool> (M, false));
    vector<vector<bool>> IsWall(N, vector<bool> (M, true));
    int dx[] = {2, -2, 0, 0};
    int dy[] = {0, 0, 2, -2};

    stack<pair<int, int>> S;
    pair<int, int> StartPos = {0, 0};
    Vis[0][0] = true;
    S.push(StartPos);
    srand(time(NULL));
    std::vector<GameObject> Walls;
    while(!S.empty()) {
        pair<int, int> Current = S.top();
        cout << Current.first << " " << Current.second << "\n";
        vector<pair<int, int>> Neighbours; 
        for(int i = 0; i < 4; i++) {
            int x = dx[i], y = dy[i];
            pair<int, int> Neighbour = {Current.first + x, Current.second + y};
            if(Valid(Neighbour, N, M)) {
                if(!Vis[Neighbour.first][Neighbour.second]) {
                    Neighbours.push_back(Neighbour);
                }
            }
        }
        int Len = Neighbours.size();
        if(Len == 0) {
            S.pop();
            continue;
        }
        int Ind = rand() % Len;
        pair<int, int> Which = Neighbours[Ind];
        Vis[Which.first][Which.second] = true;
        S.push(Which); 

        // remove wall between Which and Current
        int X, Y;
        if(Which.first == Current.first) {
            X = Which.first, Y = min(Which.second, Current.second) + 1;
        } else if(Which.second == Current.second) {
            X = min(Which.first, Current.first) + 1, Y = Which.second;
        }
        IsWall[X][Y] = false;
    }
    for(int i = 0; i < N; i++) {
        for(int j = (i % 2) ^ 1; j < M; j += 2) {
            if(IsWall[i][j]) {
                glm::vec2 Position = glm::vec2(j * this->WallSize, i * this->WallSize);
                glm::vec2 Size = glm::vec2(this->WallSize, this->WallSize);
                GameObject NewWall = GameObject(Position, Size, ResourceManager::GetTexture("wall"));
                Walls.push_back(NewWall);
            }
        }
        for(int j = (i % 2); j < M; j += 2) {
            if(!Vis[i][j]) {
                glm::vec2 Position = glm::vec2(j * this->WallSize, i * this->WallSize);
                glm::vec2 Size = glm::vec2(this->WallSize, this->WallSize);
                GameObject NewWall = GameObject(Position, Size, ResourceManager::GetTexture("wall"));
                Walls.push_back(NewWall);
            }
        }
    }
    return Walls;
}