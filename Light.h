#ifndef LIGHT_H
#define LIGHT_H

#include "GLBridge.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"

class Light : public SceneObject {
private:
  glm::vec3 color;
  float constFalloff, linearFalloff, squareFalloff;
  LightHandles handles;
  Model* model;

public:
  Light(glm::vec3 position, glm::vec3 color, float constFalloff, float linearFalloff, float squareFalloff);
  virtual void draw(float alpha); // override
  void drawModel();
};

#endif