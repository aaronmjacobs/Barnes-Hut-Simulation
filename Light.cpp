#include "Light.h"
#include "Mesh.h"

Light::Light(glm::vec3 position, glm::vec3 color, float constFalloff, float linearFalloff, float squareFalloff) {
  this->position = position;
  this->color = color;
  this->constFalloff = constFalloff;
  this->linearFalloff = linearFalloff;
  this->squareFalloff = squareFalloff;
  this->model = NULL;
  //this->handles = GLBridge::getLightHandles();
}

void Light::draw(float alpha) {
  this->handles = GLBridge::getLightHandles();
  glUniform3fv(handles.uLightPosition, 1, glm::value_ptr(position));
  glUniform3fv(handles.uLightColor, 1, glm::value_ptr(color));
  glUniform1f(handles.uLightConstFalloff, constFalloff);
  glUniform1f(handles.uLightLinearFalloff, linearFalloff);
  glUniform1f(handles.uLightSquareFalloff, squareFalloff);
  
  if (model) {
    model->setPosition(position);
    model->draw(alpha);
  }
}

void Light::drawModel() {
  Material lightMaterial;
  lightMaterial.ambient = color * 0.0f;
  lightMaterial.diffuse = color * 0.0f;
  lightMaterial.specular = color * 0.0f;
  lightMaterial.emission = color * 1.0f;
  lightMaterial.shininess = 1.0f;
  
  Mesh* sphere = Mesh::load("SimpleModels/sphere.obj", true);
  model = new Model(sphere, lightMaterial);
  model->scale(0.7f);
}