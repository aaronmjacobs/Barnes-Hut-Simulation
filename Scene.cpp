#include "Scene.h"

MatrixStack Scene::stack;

Scene::Scene() {
  camera = NULL;
}

void Scene::add(SceneObject* sceneObject) {
  sceneObjects.push_back(sceneObject);
}

bool Scene::remove(SceneObject* sceneObject) {
  for (std::vector<SceneObject*>::iterator it = sceneObjects.begin(); it != sceneObjects.end(); ++it) {
    if (*it == sceneObject) {
      sceneObjects.erase(it);
      return true;
    }
  }
  
  return false;
}

void Scene::add(Light* light) {
  lights.push_back(light);
}

bool Scene::remove(Light* light) {
  for (std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it) {
    if(*it == light) {
      lights.erase(it);
      return true;
    }
  }
  
  return false;
}


void Scene::draw(float alpha) {
  if (camera) {
    camera->updateView();
  }
  
  for (size_t i = 0; i < lights.size(); ++i) {
    lights[i]->draw(alpha);
  }
  
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    sceneObjects[i]->draw(alpha);
  }
}