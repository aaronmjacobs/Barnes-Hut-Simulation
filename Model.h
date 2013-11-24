#ifndef MODEL_H
#define MODEL_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "SceneObject.h"
#include "Mesh.h"
#include "GLBridge.h"

class Model : public SceneObject {
protected:
  // Mesh
  Mesh* mesh;

  // OpenGL handles
  TransHandles transHandles;
  MaterialHandles materialHandles;

  // Transforms
  glm::mat4 rotation;
  float scale_;

  // Material properties
  Material material;

public:
  Model(Mesh* mesh, Material material);

  void rotate(glm::vec3 axis, float angle);
  void scale(float amount);
  void resetTransforms(); // override

  void setMaterial(Material material);
  
  Mesh* getMesh() {
    return mesh;
  }
  Material getMaterial() {
    return material;
  }
  float getScale() {
    return scale_;
  }
  
  float getExtrema();

  virtual void draw(float alpha = 0.0f); // override
  
  void preDraw();
  void postDraw();
  void mainDraw(float alpha = 0.0f);
  
  bool intersectionDepth(glm::vec3 start, glm::vec3 end, float *depth);
};

#endif