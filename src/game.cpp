#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "enemy.h"
#include "player.h"
#include <iostream>
using namespace std;
// Game-related State data
SpriteRenderer  *Renderer;
PlayerObject *Player;
EnemyObject *Enemy;
float LastDoorSound = glfwGetTime();

float LastCoinTime = glfwGetTime();
int CoinFrameIndex = 0;

bool CheckCollision(GameObject &one, GameObject &two) {
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
    two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
    two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Game::Game(float width, float height, int timeLeft): Width(width), Height(height), TimeLeft(timeLeft) { 
    this->LightKeyUnpressed = true;
    this->LightOn = false;
    this->EnemyButtonPressed = false;
    this->PowerUpButtonPressed = false;
    this->LastTick = 0;
    this->GameLost = false;
    this->GameWon = false;
    this->LightScoreTimeLast = glfwGetTime();
}

void Game::LoadShaders() {
    this->SpriteShader = ResourceManager::LoadShader("../src/sprite.vs", "../src/sprite.fs", nullptr, "sprite");
    this->SpriteShader.Use();
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    this->SpriteShader.SetInteger("image", 0);
    this->SpriteShader.SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(this->SpriteShader);

    projection = glm::ortho(0.0f, this->Width, 0.0f, this->Height);
    this->TextShader = ResourceManager::LoadShader("../src/text.vs", "../src/text.fs", nullptr, "text");
    this->TextShader.Use();
    this->TextShader.SetMatrix4("projection", projection);

}
void Game::LoadTextures() {
    // ResourceManager::LoadTexture("../assets/textures/player_right.png", true, "player_right");
    // ResourceManager::LoadTexture("../assets/textures/player_left.png", true, "player_left");
    ResourceManager::LoadTexture("../assets/textures/background4.jpg", false, "background");
    ResourceManager::LoadTexture("../assets/textures/wall3.jpg", false, "wall");
    ResourceManager::LoadTexture("../assets/textures/button_unpressed_1.png", true, "button1");
    ResourceManager::LoadTexture("../assets/textures/button_unpressed_2.png", true, "button2");
    ResourceManager::LoadTexture("../assets/textures/button_pressed_1.png", true, "button_pressed1");
    ResourceManager::LoadTexture("../assets/textures/button_pressed_2.png", true, "button_pressed2");
    ResourceManager::LoadTexture("../assets/textures/door_closed.png", true, "door_closed");
    ResourceManager::LoadTexture("../assets/textures/enemy1.png", true, "enemy");
    ResourceManager::LoadTexture("../assets/textures/bomb1.png", true, "bomb");
    ResourceManager::LoadTexture("../assets/textures/lost3.png", true, "lost_screen");
    ResourceManager::LoadTexture("../assets/textures/coins_sheet3.png", true, "coin_sheet");
    ResourceManager::LoadTexture("../assets/textures/player_sprite_sheet_right.png", true, "player_sheet_right");
    ResourceManager::LoadTexture("../assets/textures/player_sprite_sheet_left.png", true, "player_sheet_left");
    ResourceManager::LoadTexture("../assets/textures/player_back.png", true, "player_back");
}
void Game::LoadLevel() {
    
    this->WallSize = 50.f;
    int ButtonSize = 26.0f;
    Level level = MakeMaze(); 
    this->Walls = level.Walls;
    this->Vis = level.Vis;

    pair<int, int> ButtonPos = level.ButtonEnemy;
    this->EnemyButton = new GameObject(glm::vec2(ButtonPos.first * this->WallSize + (this->WallSize - ButtonSize) / 2, ButtonPos.second * this->WallSize + this->WallSize - ButtonSize), glm::vec2(ButtonSize, ButtonSize), ResourceManager::GetTexture("button1"));

    ButtonPos = level.ButtonPowerUps;
    this->PowerUpButton = new GameObject(glm::vec2(ButtonPos.first * this->WallSize + (this->WallSize - ButtonSize) / 2, ButtonPos.second * this->WallSize + this->WallSize - ButtonSize), glm::vec2(ButtonSize, ButtonSize), ResourceManager::GetTexture("button2"));
    
    pair<int, int> ExitDoorPos = level.Exit;
    this->ExitDoor = new GameObject(glm::vec2(ExitDoorPos.first * this->WallSize, ExitDoorPos.second * this->WallSize), glm::vec2(15.0f, this->WallSize), ResourceManager::GetTexture("door_closed"));

    this->PlayArea = level.PlayArea;
    
    glm::vec2 EnemyPos = glm::vec2((ExitDoorPos.first - 1) * this->WallSize + (this->WallSize - PLAYER_SIZE.x) / 2,ExitDoorPos.second * this->WallSize + (this->WallSize - PLAYER_SIZE.y) / 2);
    glm::vec2 PlayerPos = glm::vec2(0.0f + (this->WallSize - PLAYER_SIZE.x) / 2, this->WallSize  + (this ->WallSize - PLAYER_SIZE.y) / 2);

    Player = new PlayerObject(PlayerPos, PLAYER_SIZE, ResourceManager::GetTexture("player_sheet_right"));
    Enemy = new EnemyObject(EnemyPos, PLAYER_SIZE, ResourceManager::GetTexture("enemy"), make_pair(ExitDoorPos.second, ExitDoorPos.first - 1), this->Vis, this->Height / this->WallSize, this->Width / this->WallSize);

    HUD = new Hud(this->TextShader);
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
    int GridX = Player->Position.x / 50.0;
    int GridY = Player->Position.y / 50.0;
    cameraPos.y = min(cameraPos.y, this->BottomBorder);
    cameraPos.y = max(cameraPos.y, this->TopBorder);
    cameraPos.x = min(cameraPos.x, this->LeftBorder);
    cameraPos.x = max(cameraPos.x, this->RightBorder);

    glm::mat4 view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
    
    // view = glm::mat4(1.0f);
    this->SpriteShader.Use();
    this->SpriteShader.SetMatrix4("view", view);
    this->SpriteShader.SetVector2f("PlayerPos", Player->Position);

    if(this->GameLost) {
        this->cameraPos = glm::vec3(0.0f + (this->WallSize - PLAYER_SIZE.x) / 2, this->WallSize  + (this ->WallSize - PLAYER_SIZE.y) / 2, -625.026f);
        this->cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
        this->cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);
        this->SpriteShader.SetInteger("LightOn", 1);
        float AspectRatio = 502.0f / 891.0f;
        float Width = 880.0f;
        Renderer->drawSprite(ResourceManager::GetTexture("lost_screen"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Width * AspectRatio), 0.0f);
        HUD->GameLost(Player->Score);
        // HUD->Display(Player->Lives, this->PowerUpButtonPressed, this->EnemyButtonPressed, this->LightOn, this->TimeLeft, Player->Score);
        return;
    }
    if(this->GameWon) {
        this->cameraPos = glm::vec3(0.0f + (this->WallSize - PLAYER_SIZE.x) / 2, this->WallSize  + (this ->WallSize - PLAYER_SIZE.y) / 2, -625.026f);
        this->cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
        this->cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);
        this->SpriteShader.SetInteger("LightOn", 1);
        float AspectRatio = 502.0f / 891.0f;
        float Width = 880.0f;
        Renderer->drawSprite(ResourceManager::GetTexture("lost_screen"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Width * AspectRatio), 0.0f);
        HUD->GameWon(Player->Score);
        return;
    }

    if(this->LightOn) {
        this->SpriteShader.SetInteger("LightOn", 1);
    } else {
        this->SpriteShader.SetInteger("LightOn", 0);
    }
    Renderer->drawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    
    for(GameObject Wall: this->Walls) {
        Wall.Draw(*Renderer);
    }
    // this->ExitDoor->Rotation = 100;
    if(!this->PowerUpButtonPressed || !this->EnemyButtonPressed) {
        this->ExitDoor->Draw(*Renderer);
    }

    float CurTime = glfwGetTime();
    if(CurTime - LastCoinTime >= FrameSpeed) {
        CoinFrameIndex = (1 + CoinFrameIndex) % 6;
        LastCoinTime = CurTime;
    }
    float CoinStart = 0 + (1.0 / CoinFrames) * CoinFrameIndex;
    float CoinEnd = (1.0 / CoinFrames) + (1.0 / CoinFrames) * CoinFrameIndex;
    for(GameObject& item: this->PowerUps) {
        if(item.Destroyed == false) {
            if(item.Score > 0) {
                item.DrawSheet(*Renderer, CoinStart, CoinEnd);
            } else {
                item.Draw(*Renderer);
            }
        }
    }
    if(Player->Animation) {
        Player->DrawSheet(*Renderer, PlayerFramePositions[Player->FrameIndex].first, PlayerFramePositions[Player->FrameIndex].second, true);
    } else {
        Player->Draw(*Renderer);
    }

    if(Enemy->Alive == true) {
        Enemy->Draw(*Renderer);
    }
    
    this->EnemyButton->Draw(*Renderer);
    this->PowerUpButton->Draw(*Renderer);
    HUD->Display(Player->Lives, this->PowerUpButtonPressed, this->EnemyButtonPressed, this->LightOn, this->TimeLeft, Player->Score);
}
bool Game::CollisionWithWall() {
    // return false;
    for(GameObject Wall: this->Walls) {
        if(CheckCollision(Wall, *Player)) {
            return true;
        }
    }
    return false;
}
bool Game::CollisionWithDoor() {
    if(this->ExitDoor->Destroyed == true) {
        return false;
    }
    bool result = CheckCollision(*Player, *this->ExitDoor);
    if(result) {
        float CurTime = glfwGetTime();
        if(CurTime - LastDoorSound >= 1.0f) {
            LastDoorSound = CurTime;
            system("aplay ../assets/sounds/locked_door.wav &");
        }
        return true;
    }
    return false;
}
void Game::ProcessInput(float dt) {
    if(this->GameLost || this->GameWon) {
        return;
    }
    glm::vec2 Direction1 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance1 = glm::length(Direction1);
    float velocity = PLAYER_VELOCITY * dt;
    if(this->Keys[GLFW_KEY_F] == false) {
        this->LightKeyUnpressed = true;
    } else if(this->LightKeyUnpressed == true) {
        this->LightKeyUnpressed = false;
        this->LightOn = !this->LightOn;
        system("aplay ../assets/sounds/torch.wav &");
    }
    float CurTime = glfwGetTime();
    if(this->Keys[GLFW_KEY_D]) {
        Player->Animation = true;
        Player->Sprite = ResourceManager::GetTexture("player_sheet_right");
        if(Player->Position.x <= this->Width - Player->Size.x) {
            Player->Position.x += velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.x -= velocity;
            } else {
                Player->Ticks = (Player->Ticks + 1) % (Player->FrameSpeed);
                if(Player->Ticks == 0) {
                    Player->FrameIndex = (Player->FrameIndex + 1) % 4;
                }
            }
        } else {
            Player->Position.x = this->Width - Player->Size.x;
        }
    } else if(this->Keys[GLFW_KEY_A]) {
        Player->Animation = true;
        Player->Sprite = ResourceManager::GetTexture("player_sheet_left");
        if(Player->Position.x >= 0.0f) {
            Player->Position.x -= velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.x += velocity;
            } else {
                Player->Ticks = (Player->Ticks + 1) % (Player->FrameSpeed);
                if(Player->Ticks == 0) {
                    Player->FrameIndex = (Player->FrameIndex + 1) % 4;
                }
            }
        } else {
            Player->Position.x = 0.0f;
        }
    } else if(this->Keys[GLFW_KEY_W]) {
        Player->Animation = true;
        // Player->Sprite = ResourceManager::GetTexture("player_back");
        if(Player-> Position.y >= 0.0f) {
            Player->Position.y -= velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.y += velocity;
            }
        } else {
            Player->Position.y = 0.0f;
        }
    } else if(this->Keys[GLFW_KEY_S]) {
        Player->Animation = true;
        Player->Sprite = ResourceManager::GetTexture("player_sheet_right");
        if(Player->Position.y <= this->Height - Player->Size.y) {
            Player->Position.y += velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.y -= velocity;
            }
        } else {
            Player->Position.y = this->Height - Player->Size.y;
        }
    }
    glm::vec2 Direction2 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance2 = glm::length(Direction2);
    if(Distance2 > Distance1 && Distance2 >= 150) {
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
    // cout << Player->Position.x << " " << Player->Position.y << "  " << this->EnemyButton->Position.x << " " << this->EnemyButton->Position.y << "\n";
    if(this->EnemyButtonPressed == false && CheckCollision(*Player, *this->EnemyButton)) {
        this->EnemyButtonPressed = true;
        this->EnemyButton->Sprite = ResourceManager::GetTexture("button_pressed1");
        this->EnemyButton->Size.y -= 7.0f;
        this->EnemyButton->Position.y += 7.0f;
        Enemy->Alive = false;
        system("aplay ../assets/sounds/button.wav &");

        // open the door if both tasks are completed
        if(this->PowerUpButtonPressed == true) {
            system("aplay ../assets/sounds/door_opening.wav &");
            this->ExitDoor->Destroyed = true;
        }
    }
    if(this->PowerUpButtonPressed == false && CheckCollision(*Player, *this->PowerUpButton)) {
        this->PowerUpButtonPressed = true;
        this->PowerUpButton->Sprite = ResourceManager::GetTexture("button_pressed2");
        this->PowerUpButton->Size.y -= 7.0f;
        this->PowerUpButton->Position.y += 7.0f;
        
        // open the door if both tasks are completed
        if(this->EnemyButtonPressed == true) {
            system("aplay ../assets/sounds/door_opening.wav &");
            this->ExitDoor->Destroyed = true;
        }
        random_shuffle(this->PlayArea.begin(), this->PlayArea.end());
        for(int i = 0; i < 10; i++) {
            glm::vec2 Size = glm::vec2(this->WallSize - 14, this->WallSize - 14);
            glm::vec2 Position = glm::vec2(7 + this->PlayArea[i].first, 7 + this->PlayArea[i].second);
            GameObject NewCoin = GameObject(Position, Size, ResourceManager::GetTexture("coin_sheet"));
            NewCoin.Score = 10;
            PowerUps.push_back(NewCoin);
        }
        for(int i = 10; i < 20; i++) {
            glm::vec2 Size = glm::vec2(this->WallSize - 14, this->WallSize - 14);
            glm::vec2 Position = glm::vec2(7 + this->PlayArea[i].first, 7 + this->PlayArea[i].second);
            GameObject NewBomb = GameObject(Position, Size, ResourceManager::GetTexture("bomb"));
            NewBomb.Score = -10;
            PowerUps.push_back(NewBomb);
        }
        system("aplay ../assets/sounds/button.wav &");
    }
    for(GameObject& item: PowerUps) {
        if(!item.Destroyed && CheckCollision(item, *Player)) {
            item.Destroyed = true;
            Player->Score += item.Score;
            if(item.Score > 0) {
                system("aplay ../assets/sounds/coin.wav &");
            } else {
                system("aplay ../assets/sounds/explosion.wav &");
            }
        }
    }
    
}

void Game::Update(float dt) {
    if(this->GameLost || this->GameWon) {
        return;
    }
    if(Enemy->Alive == true) {
        if(glfwGetTime() - Enemy->LastAttack >= GapBetweenAttacks && CheckCollision(*Enemy, *Player)) {
            Enemy->LastAttack = glfwGetTime();
            Player->Lives -= 1;
            system("aplay ../assets/sounds/knife_stab.wav &");
        }
        UpdateEnemy(dt);
        if(glfwGetTime() - Enemy->LastAttack >= GapBetweenAttacks && CheckCollision(*Enemy, *Player)) {
            Enemy->LastAttack = glfwGetTime();
            Player->Lives -= 1;
            system("aplay ../assets/sounds/knife_stab.wav &");
        }
    }
    
    // check if player reached the exit
    pair<int, int> CurCell = make_pair(Player->Position.x / this->WallSize, Player->Position.y / this->WallSize);
    pair<int, int> ExitCell = make_pair(this->ExitDoor->Position.x / this->WallSize, this->ExitDoor->Position.y / this->WallSize);
    if(CurCell == ExitCell) {
        system("aplay ../assets/sounds/win.wav &");
        this->GameWon = true;
        return;
    }
    

    float CurTime = glfwGetTime();
    if(CurTime - LastTick >= 1) {
        LastTick = CurTime;
        this->TimeLeft -= 1;
    }
    // check if time over
    if(this->TimeLeft <= 0 || Player->Lives <= 0) {
        system("aplay ../assets/sounds/lost.wav &");
        this->GameLost = true;
        return;
    }
    if(!this->LightOn) {
        if(CurTime - this->LightScoreTimeLast >= 1) {
            this->LightScoreTimeLast = CurTime;
            Player->Score += 1;
        }
    } else {
        this->LightScoreTimeLast = CurTime;
    }
}

void Game::UpdateEnemy(float dt) {
    if(Enemy->Alive == false) {
        return;
    }
    float velocity = PLAYER_VELOCITY * dt;
    pair<int, int> EnemyCurPosition = make_pair(Enemy->Position.y / this->WallSize, Enemy->Position.x / this->WallSize);
    glm::vec2 GoToPixel = glm::vec2(Enemy->GoToCell.second *this->WallSize + (this->WallSize - PLAYER_SIZE.x) / 2, Enemy->GoToCell.first * this->WallSize + (this->WallSize - PLAYER_SIZE.y) / 2);
    if(Enemy->Position != GoToPixel) {
        glm::vec2 Direction = normalize(GoToPixel - Enemy->Position);
        if(Enemy->Position.x != GoToPixel.x) {
            velocity = min(velocity, abs(Enemy->Position.x - GoToPixel.x));
        } else if(Enemy->Position.y != GoToPixel.y) {
            velocity = min(velocity, abs(Enemy->Position.y - GoToPixel.y));
        }
        Enemy->Position += glm::vec2(0.5 * velocity * Direction.x, 0.5 * velocity * Direction.y);
        return;
    }
    pair<int, int> PlayerCurPosition = make_pair(Player->Position.y / this->WallSize, Player->Position.x / this->WallSize);
    Enemy->GoToCell = Enemy->GoTo[PlayerCurPosition.first][PlayerCurPosition.second][EnemyCurPosition.first][EnemyCurPosition.second];
}

bool Valid(pair<int, int> P, int N, int M) {
    return P.first >= 1 && P.first < N - 1 && P.second >= 1 && P.second < M - 1;
}
Level Game::MakeMaze() {
    Level level;
    int N = this->Height / this->WallSize;
    int M = this->Width / this->WallSize;
    vector<vector<bool>> Vis(N, vector<bool> (M, false));
    int dx[] = {2, -2, 0, 0};
    int dy[] = {0, 0, 2, -2};

    stack<pair<int, int>> S;
    pair<int, int> StartPos = {1, 1};
    Vis[1][1] = true;
    S.push(StartPos);
    srand(time(NULL));
    std::vector<GameObject> Walls;
    while(!S.empty()) {
        pair<int, int> Current = S.top();
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
        int X, Y;
        if(Which.first == Current.first) {
            X = Which.first, Y = min(Which.second, Current.second) + 1;
        } else if(Which.second == Current.second) {
            X = min(Which.first, Current.first) + 1, Y = Which.second;
        }
        Vis[X][Y] = true;
    }

    // Exit Location
    vector<int> PossibleExitPoints;
    for(int i = 1; i < N - 1; i++) {
        if(Vis[i][M - 2]) {
            PossibleExitPoints.push_back(i);
        }
    }
    int ExitPoint = PossibleExitPoints[rand() % PossibleExitPoints.size()];
    level.Exit = make_pair(M - 1, ExitPoint);
    

    // Button Location
    vector<pair<int, int>> WallBelow, NoWallBelow;              // prefer block with wall below
    for(int i = N / 2; i < N - 2; i++) {
        for(int j = M / 2; j < M - 1; j++) {
            if(Vis[i][j] && !Vis[i + 1][j]) {
                WallBelow.push_back({i, j});
            } else if(Vis[i][j]) {
                NoWallBelow.push_back({i, j});
            }
        }
    }
    random_shuffle(WallBelow.begin(), WallBelow.end());
    random_shuffle(NoWallBelow.begin(), NoWallBelow.end());

    if(!WallBelow.empty()) {
        pair<int, int> point = WallBelow.back();
        WallBelow.pop_back();
        level.ButtonEnemy = {point.second, point.first};
    } else {
        pair<int, int> point = NoWallBelow.back();
        NoWallBelow.pop_back();
        level.ButtonEnemy = {point.second, point.first};
    }

    if(!WallBelow.empty()) {
        pair<int, int> point = WallBelow.back();
        WallBelow.pop_back();
        level.ButtonPowerUps = {point.second, point.first};
    } else {
        pair<int, int> point = NoWallBelow.back();
        NoWallBelow.pop_back();
        level.ButtonPowerUps = {point.second, point.first};
    }

    Vis[1][0] = true;
    Vis[level.Exit.second][level.Exit.first] = true;
    vector<pair<float, float>> PlayArea;
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            if(!Vis[i][j]) {
                glm::vec2 Size = glm::vec2(this->WallSize, this->WallSize);
                glm::vec2 Position = glm::vec2(j * this->WallSize, i * this->WallSize);
                GameObject NewWall = GameObject(Position, Size, ResourceManager::GetTexture("wall"));
                Walls.push_back(NewWall);
            } else {
                PlayArea.push_back({j * this->WallSize, i * this->WallSize});
            }
        }
    }   
    level.Walls = Walls;   
    level.PlayArea = PlayArea;
    level.Vis = Vis;
    return level;
}