#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include <bits/stdc++.h>
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
    ResourceManager::LoadTexture("../assets/textures/background3.jpg", false, "background");
    // this->cameraPos = glm::vec3(393.036f, 352.741f, 625.026f);
}
void Game::LoadLevel() {
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
}

void Game::CameraInit() {
    this->cameraPos = glm::vec3(Player->Position.x, Player->Position.y  , 625.026f);
    this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

    this->BottomBorder = 740.f;
    this->TopBorder = 255.0f;

    this->RightBorder = 350.0f;
    this->LeftBorder = 1213.0f;

}

void Game::Render() {
    glm::mat4 view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
    
    // view = glm::mat4(1.0f);
    this->SpriteShader.Use();
    this->SpriteShader.SetMatrix4("view", view);
    Renderer->drawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    Player->Draw(*Renderer);
}

void Game::ProcessInput(float dt) {
    glm::vec2 Direction1 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance1 = glm::length(Direction1);
    float velocity = PLAYER_VELOCITY * dt;
    if(this->Keys[GLFW_KEY_A]) {
        if(Player->Position.x <= this->Width - Player->Size.x) {
            Player->Position.x += velocity;
        }
    } else if(this->Keys[GLFW_KEY_D]) {
        if(Player->Position.x >= 0.0f) {
            Player->Position.x -= velocity;
        }
    } else if(this->Keys[GLFW_KEY_W]) {
        if(Player-> Position.y >= 0.0f) {
            Player->Position.y -= velocity;
        }
    } else if(this->Keys[GLFW_KEY_S]) {
        if(Player->Position.y <= this->Height - Player->Size.y) {
            Player->Position.y += velocity;
        }
    }
    glm::vec2 Direction2 = Player->Position - glm::vec2(this->cameraPos.x, this->cameraPos.y);
    float Distance2 = glm::length(Direction2);
    if(Distance2 > Distance1 && Distance2 >= 210) {
        Direction2 = glm::normalize(Direction2);
        cameraPos.x += Direction2.x;
        cameraPos.y += Direction2.y;
    }
    cameraPos.y = min(cameraPos.y, this->BottomBorder);
    cameraPos.y = max(cameraPos.y, this->TopBorder);
    cameraPos.x = min(cameraPos.x, this->LeftBorder);
    cameraPos.x = max(cameraPos.x, this->RightBorder);

}