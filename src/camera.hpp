#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  0.01f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
  // camera Attributes
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;
  // euler Angles
  float yaw;
  float pitch;
  // camera options
  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  // constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
  {
    this->position = position;
    worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
  }
  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
  {
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 getViewMatrix()
  {
    return glm::lookAt(position, this->position + front, up);
  }

  // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
  void processKeyboard(CameraMovement direction, float deltaTime = 1)
  {
    float velocity = movementSpeed * deltaTime;
    switch (direction)
    {
    case FORWARD:
      this->position += front * velocity;
      break;
    case BACKWARD:
      this->position -= front * velocity;
      break;
    case LEFT:
      this->position -= right * velocity;
      break;
    case RIGHT:
      this->position += right * velocity;
      break;
    case UP:
      this->position += up * velocity;
      break;
    case DOWN:
      this->position -= up * velocity;
      break;
    }
  }

  // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
  void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
  {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    this->yaw   += xoffset;
    this->pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
      if (this->pitch > 89.0f)
        this->pitch = 89.0f;
      if (this->pitch < -89.0f)
        this->pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
  }

  // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void processMouseScroll(float yoffset)
  {
    this->zoom -= (float)yoffset;
    if (this->zoom < 1.0f)
      this->zoom = 1.0f;
    if (this->zoom > 45.0f)
      this->zoom = 45.0f;
  }

private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors()
  {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    this->right = glm::normalize(glm::cross(this->front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->up    = glm::normalize(glm::cross(this->right, this->front));
  }
};
