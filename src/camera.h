#ifndef CAMERA_H
#define CAMERA_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<vector>

enum camera_movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
    public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    float yaw;
    float pitch;
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
        this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
        this->position = glm::vec3(posX, posY, posZ);
        this->world_up = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix(){
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }

    void ProcessKeyboard(camera_movement direction, float deltaTime) {
        float velocity = this->movement_speed * deltaTime;
        if (direction == FORWARD) {
            this->position += this->front * velocity;
        } else if (direction == BACKWARD) {
            this->position -= this->front * velocity;
        } else if (direction == LEFT) {
            this->position -= this->right * velocity;
        } else if (direction == RIGHT) {
            this->position += this->right * velocity;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= this->mouse_sensitivity;
        yoffset *= this->mouse_sensitivity;

        this->yaw   += xoffset;
        this->pitch += yoffset;

        if (constrainPitch)
        {
            if (this->pitch > 89.0f) {
                this->pitch = 89.0f;
            }
            if (this->pitch < -89.0f) {
                this->pitch = -89.0f;
            }
        }
        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset) {
        this->zoom -= (float)yoffset;
        if (this->zoom < 1.0f) {
            this->zoom = 1.0f;
        }
        if (this->zoom > 45.0f) {
            this->zoom = 45.0f; 
        }
    }

    private:
    void updateCameraVectors()
    {
        glm::vec3 Front;
        Front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        Front.y = sin(glm::radians(this->pitch));
        Front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        this->front = glm::normalize(Front);
        this->right = glm::normalize(glm::cross(this->front, this->world_up));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};
#endif
