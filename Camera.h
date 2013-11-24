#ifndef CAMERA_H
#define CAMERA_H

#include "GLBridge.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class Camera {
private:
  GLint h_uViewMatrix, h_uCameraPos;
  glm::vec3 position;
  float phi, theta;
  glm::vec3 front() {
    return glm::vec3(cos(phi) * cos(theta),
                     sin(phi),
                     cos(phi) * cos(90 - theta));
  }

public:
  Camera(GLint h_uViewMatrix, GLint h_uCameraPos);
  void resetView();
  void translate(glm::vec3 trans);
  void fly(float amount);
  void walk(float amount);
  void strafe(float amount);
  void rotate(float phi, float theta);
  void updateView();
  glm::mat4 getViewMatrix();
};

#endif