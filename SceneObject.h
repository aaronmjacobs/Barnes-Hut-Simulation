#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "glm/glm.hpp"

class SceneObject {
protected:
  glm::vec3 position;

public:
  virtual void translate(glm::vec3 trans);
  virtual void setPosition(glm::vec3 pos);
  virtual void resetTransforms();
  virtual void draw(float lerp = 0.0f) = 0;
  glm::vec3 getPosition() {
    return position;
  }
};

#endif