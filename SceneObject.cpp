#include "SceneObject.h"

void SceneObject::translate(glm::vec3 trans) {
  position += trans;
}

void SceneObject::setPosition(glm::vec3 pos) {
  position = pos;
}

void SceneObject::resetTransforms() {
  position = glm::vec3();
}