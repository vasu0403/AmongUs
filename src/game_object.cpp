#include "game_object.h"

GameObject::GameObject() : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), Destroyed(false), Score(0){ }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity) : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), Destroyed(false), Score(0){ }

void GameObject::Draw(SpriteRenderer &renderer) {
    renderer.drawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

void GameObject::DrawSheet(SpriteRenderer &renderer, float start, float end, bool vertical) {
    if(vertical) {
        renderer.drawSpriteSheetVertical(this->Sprite, this->Position, this->Size, this->Rotation, start, end, this->Color);
        return;
    }
    renderer.drawSpriteSheet(this->Sprite, this->Position, this->Size, this->Rotation, start, end, this->Color);
}