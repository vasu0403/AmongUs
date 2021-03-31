#include "player.h"
using namespace std;
PlayerObject::PlayerObject(): GameObject(), Lives(3), Score(0), Ticks(0), FrameIndex(0), FrameSpeed(150){
}
PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity): GameObject(pos, size, sprite, color, velocity), Lives(3), Score(0), Ticks(0), FrameIndex(0), FrameSpeed(150){
}
void PlayerObject::Draw(SpriteRenderer &renderer) {
    renderer.drawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}