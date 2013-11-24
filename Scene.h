#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "SceneObject.h"
#include "Light.h"
#include "MatrixStack.h"
#include "Camera.h"

class Scene {
private:
  Camera* camera;
  std::vector<SceneObject*> sceneObjects;
  std::vector<Light*> lights;

public:
  static MatrixStack stack;

  Scene();
  void add(SceneObject* sceneObject);
  bool remove(SceneObject* sceneObject);
  void setCamera(Camera* camera) {
    this->camera = camera;
  }
  Camera* getCamera() {
    return camera;
  }
  void add(Light* light);
  bool remove(Light* light);
  int getNumLights() {
    return lights.size();
  }
  void draw(float alpha = 0.0f);
};

#endif