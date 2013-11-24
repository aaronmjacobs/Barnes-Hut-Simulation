#include <iostream>
#include "Particle.h"
#include "QuadTree.h"

#include "Engine.h"

Material genMaterial(glm::vec3 baseColor, float shininess) {
  Material material;
  material.ambient = baseColor * 0.1f;
  material.diffuse = baseColor * 0.6f;
  material.specular = glm::vec3(0.2f);
  material.emission = glm::vec3(0.0f);
  material.shininess = shininess;
  
  return material;
}

int main(int argc, char* argv[]) {
  Engine engine(argc, argv);
  engine.init(1280, 720, "Engine Test");
  
  //Material grayMaterial = genMaterial(glm::vec3(0.3f, 0.3f, 0.01f), 10.0f);
  Material particleMaterial;
  particleMaterial.diffuse = glm::vec3(0.9f, 0.9f, 0.01f);
  Scene scene;
  
  Light light(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.1f, 0.005f, 0.001f);
  scene.add(&light);
  
  Camera camera(GLBridge::getUViewMatrix(), GLBridge::getUCameraPos());
  camera.fly(-20.0f);
  scene.setCamera(&camera);
  
  Particle::model = new Model(Mesh::load("SimpleModels/sphere.obj", false), particleMaterial);
  Particle::model->scale(0.02f);
  //scene.add(Particle::model);
  
  engine.setScene(&scene);
  
  engine.start();
  
  return 0;
}