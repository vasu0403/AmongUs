#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "enemy.h"
#include <iostream>
using namespace std;
// Game-related State data
SpriteRenderer  *Renderer;
GameObject *Player;
EnemyObject *Enemy;
unsigned int CharacterVAO, CharacterVBO;

std::map<GLchar, Character> Characters;

void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(CharacterVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, CharacterVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

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

Game::Game(float width, float height): Width(width), Height(height) { 
    this->LightKeyUnpressed = true;
    this->LightOn = false;
    this->EnemyButtonPressed = false;
    this->PowerUpButtonPressed = false;
}

void Game::LoadShaders() {
    this->SpriteShader = ResourceManager::LoadShader("../source/sprite.vs", "../source/sprite.fs", nullptr, "sprite");
    this->SpriteShader.Use();
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    this->SpriteShader.SetInteger("image", 0);
    this->SpriteShader.SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(this->SpriteShader);

    projection = glm::ortho(0.0f, this->Width, 0.0f, this->Height);
    this->TextShader = ResourceManager::LoadShader("../source/text.vs", "../source/text.fs", nullptr, "text");
    this->TextShader.Use();
    this->TextShader.SetMatrix4("projection", projection);

}
void Game::LoadTextures() {
    ResourceManager::LoadTexture("../assets/textures/player3.png", true, "player");
    ResourceManager::LoadTexture("../assets/textures/background4.jpg", false, "background");
    ResourceManager::LoadTexture("../assets/textures/wall3.jpg", false, "wall");
    ResourceManager::LoadTexture("../assets/textures/button_unpressed_1.png", true, "button1");
    ResourceManager::LoadTexture("../assets/textures/button_unpressed_2.png", true, "button2");
    ResourceManager::LoadTexture("../assets/textures/button_pressed_1.png", true, "button_pressed1");
    ResourceManager::LoadTexture("../assets/textures/button_pressed_2.png", true, "button_pressed2");
    ResourceManager::LoadTexture("../assets/textures/door_closed.png", true, "door_closed");
    ResourceManager::LoadTexture("../assets/textures/enemy1.png", true, "enemy");
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

    vector<pair<float, float>> PlayArea = level.PlayArea;
    
    glm::vec2 EnemyPos = glm::vec2((ExitDoorPos.first - 1) * this->WallSize + (this->WallSize - PLAYER_SIZE.x) / 2,ExitDoorPos.second * this->WallSize + (this->WallSize - PLAYER_SIZE.y) / 2);
    glm::vec2 PlayerPos = glm::vec2(0.0f + (this->WallSize - PLAYER_SIZE.x) / 2, this->WallSize  + (this ->WallSize - PLAYER_SIZE.y) / 2);

    Player = new GameObject(PlayerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
    Enemy = new EnemyObject(EnemyPos, PLAYER_SIZE, ResourceManager::GetTexture("enemy"), make_pair(ExitDoorPos.second, ExitDoorPos.first - 1), this->Vis, this->Height / this->WallSize, this->Width / this->WallSize);
}

void Game::LoadFonts() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
    FT_Face face;
    if (FT_New_Face(ft, "../assets/fonts/Antonio-Regular.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
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
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
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

    glGenVertexArrays(1, &CharacterVAO);
    glGenBuffers(1, &CharacterVBO);
    glBindVertexArray(CharacterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, CharacterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    if(this->LightOn) {
        this->SpriteShader.SetInteger("LightOn", 1);
    } else {
        this->SpriteShader.SetInteger("LightOn", 0);
    }
    Renderer->drawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    
    for(GameObject Wall: this->Walls) {
        Wall.Draw(*Renderer);
    }
    this->ExitDoor->Draw(*Renderer);
    
    Player->Draw(*Renderer);

    Enemy->Draw(*Renderer);

    this->EnemyButton->Draw(*Renderer);
    this->PowerUpButton->Draw(*Renderer);

    RenderText(this->TextShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    RenderText(this->TextShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

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
    return CheckCollision(*Player, *this->ExitDoor);
}
void Game::ProcessInput(float dt) {
    glm::vec2 Direction1 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance1 = glm::length(Direction1);
    float velocity = PLAYER_VELOCITY * dt;
    if(this->Keys[GLFW_KEY_E]) {
        this->cameraPos.z -= velocity;
    }
    if(this->Keys[GLFW_KEY_F] == false) {
        this->LightKeyUnpressed = true;
    } else if(this->LightKeyUnpressed == true) {
        this->LightKeyUnpressed = false;
        this->LightOn = !this->LightOn;
    }
    if(this->Keys[GLFW_KEY_D]) {
        if(Player->Position.x <= this->Width - Player->Size.x) {
            Player->Position.x += velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.x -= velocity;
            }
        } else {
            Player->Position.x = this->Width - Player->Size.x;
        }
    } else if(this->Keys[GLFW_KEY_A]) {
        if(Player->Position.x >= 0.0f) {
            Player->Position.x -= velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.x += velocity;
            }
        } else {
            Player->Position.x = 0.0f;
        }
    } else if(this->Keys[GLFW_KEY_W]) {
        if(Player-> Position.y >= 0.0f) {
            Player->Position.y -= velocity;
            if(CollisionWithWall() || CollisionWithDoor()) {
                Player->Position.y += velocity;
            }
        } else {
            Player->Position.y = 0.0f;
        }
    } else if(this->Keys[GLFW_KEY_S]) {
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
    }
    if(this->PowerUpButtonPressed == false && CheckCollision(*Player, *this->PowerUpButton)) {
        this->PowerUpButtonPressed = true;
        this->PowerUpButton->Sprite = ResourceManager::GetTexture("button_pressed2");
        this->PowerUpButton->Size.y -= 7.0f;
        this->PowerUpButton->Position.y += 7.0f;
    }
}

void Game::UpdateEnemy(float dt) {
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
                glm::vec2 Position = glm::vec2(j * this->WallSize, i * this->WallSize);
                glm::vec2 Size = glm::vec2(this->WallSize, this->WallSize);
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