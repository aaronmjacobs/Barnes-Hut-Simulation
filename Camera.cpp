#include "Camera.h"
#include "glm/gtc/type_ptr.hpp"

#define PI 3.1415926535897
// A little less than PI / 2
#define VERT_BOUNDS 1.5

Camera::Camera(GLint h_uViewMatrix, GLint h_uCameraPos) {
  this->h_uViewMatrix = h_uViewMatrix;
  this->h_uCameraPos = h_uCameraPos;
  
  resetView();
}

void Camera::resetView() {
  position = glm::vec3(0.0f, 0.0f, 0.0f);
  phi = 0.0f;
  theta = -PI/2.0f;
}

void Camera::translate(glm::vec3 trans) {
  position += trans;
}

void Camera::fly(float amount) {
  glm::vec3 forward = glm::normalize(front());
  
  translate(forward * amount);
}

void Camera::walk(float amount) {
  glm::vec3 forward = front();
  forward.y = 0.0f;
  forward = glm::normalize(forward);
  
  translate(forward * amount);
}

void Camera::strafe(float amount) {
  glm::vec3 forward = front();
  forward.y = 0.0f;
  forward = glm::normalize(forward);
  glm::vec3 right = glm::cross(forward, glm::vec3(0.0, 1.0, 0.0));
  
  translate(right * amount);
}

void Camera::rotate(float phi, float theta) {
  if(this->phi + phi < VERT_BOUNDS && this->phi + phi > -VERT_BOUNDS)
  {
    this->phi += phi;
  }
  this->theta += theta;
}

void Camera::updateView() {
  glm::mat4 v = getViewMatrix();

  glUniformMatrix4fv(h_uViewMatrix, 1, GL_FALSE, glm::value_ptr(v));
  glUniform3fv(h_uCameraPos, 1, glm::value_ptr(position));
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, // Camera position
                     position + front(), // What we're looking at
                     glm::vec3(0.0, 1.0, 0.0)); // Up vector
}