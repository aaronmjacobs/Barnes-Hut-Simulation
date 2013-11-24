#include "Model.h"
#include "Scene.h"
#include "Maths.h"

Model::Model(Mesh* mesh,
             Material material) {
  this->mesh = mesh;
  
  resetTransforms();

  this->transHandles = GLBridge::getTransHandles();
  this->materialHandles = GLBridge::getMaterialHandles();
  setMaterial(material);
}

void Model::rotate(glm::vec3 axis, float angle) {
  glm::mat4 r = glm::rotate(glm::mat4(1.0), angle, axis);
  rotation = r * rotation;
}

void Model::scale(float amount) {
  scale_ *= amount;
}

void Model::resetTransforms() {
  position = glm::vec3();
  rotation = glm::mat4(1.0f);
  scale_ = 1.0f;
}

void Model::setMaterial(Material material) {
  this->material = material;
}

float Model::getExtrema() {
  Bounds bounds = mesh->bounds;
  float maxExtrema = Maths::max(fabs(bounds.min.x), fabs(bounds.min.y));
  maxExtrema = Maths::max(maxExtrema, fabs(bounds.min.z));
  maxExtrema = Maths::max(maxExtrema, fabs(bounds.max.x));
  maxExtrema = Maths::max(maxExtrema, fabs(bounds.max.y));
  maxExtrema = Maths::max(maxExtrema, fabs(bounds.max.z));
  
  maxExtrema *= scale_;
  
  return maxExtrema;
}

void Model::preDraw() {
  // Set material properties
  glUniform3fv(materialHandles.uColor, 1, glm::value_ptr(material.diffuse));

  // Bind vbo
  glEnableVertexAttribArray(transHandles.aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffObj);
  glVertexAttribPointer(transHandles.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Bind ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffObj);
}

void Model::postDraw() {
  // Clean up
  glDisableVertexAttribArray(transHandles.aPosition);
}

void Model::mainDraw(float alpha) {
  Scene::stack.push();
  
  // Set transform / normal matrices
  glm::mat4* transform = Scene::stack.getMatrix();
  (*transform) *= glm::translate(position) * glm::scale(glm::vec3(scale_, scale_, scale_));
  glUniformMatrix4fv(transHandles.uModelMatrix, 1, GL_FALSE, glm::value_ptr(*transform));
  
  // Draw
  glDrawElements(GL_TRIANGLES, mesh->indexCount * 3, GL_UNSIGNED_INT, 0);
  
  Scene::stack.pop();
}

void Model::draw(float alpha) {
  Scene::stack.push();
  
  // Set material properties
  glUniform3fv(materialHandles.uAmbientColor, 1, glm::value_ptr(material.ambient));
  glUniform3fv(materialHandles.uDiffuseColor, 1, glm::value_ptr(material.diffuse));
  glUniform3fv(materialHandles.uSpecularColor, 1, glm::value_ptr(material.specular));
  glUniform3fv(materialHandles.uEmissionColor, 1, glm::value_ptr(material.emission));
  glUniform1f(materialHandles.uShininess, material.shininess);
  
  // Set transform / normal matrices
  glm::mat4 transMatrix = glm::translate(position);
  glm::mat4 scaleMatrix = glm::scale(glm::vec3(scale_, scale_, scale_));
  glm::mat4* transform = Scene::stack.getMatrix();
  (*transform) *= transMatrix * rotation * scaleMatrix;
  glm::mat4 normal = glm::transpose(glm::inverse(*transform));
  glUniformMatrix4fv(transHandles.uModelMatrix, 1, GL_FALSE, glm::value_ptr(*transform));
  glUniformMatrix4fv(transHandles.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal));

  // Bind vbo
  glEnableVertexAttribArray(transHandles.aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffObj);
  glVertexAttribPointer(transHandles.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Bind ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffObj);

  // Bind normals
  glEnableVertexAttribArray(transHandles.aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBuffObj);
  glVertexAttribPointer(transHandles.aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Draw
  glDrawElements(GL_TRIANGLES, mesh->indexCount * 3, GL_UNSIGNED_INT, 0);

  // Clean up
  glDisableVertexAttribArray(transHandles.aPosition);
  glDisableVertexAttribArray(transHandles.aNormal);
  
  Scene::stack.pop();
}

bool Model::intersectionDepth(glm::vec3 start, glm::vec3 end, float *depth) {
  glm::vec3 mouseToModel = getPosition() - start;
  float toModelLength = glm::length(mouseToModel);
  
  glm::vec3 ray = glm::normalize(end - start) * toModelLength;
  glm::vec3 between = ray - mouseToModel;
  float dist = glm::length(between);
  
  float radius = mesh->bounds.max.x > mesh->bounds.max.y ? mesh->bounds.max.x : mesh->bounds.max.y;
  radius = radius > mesh->bounds.max.z ? radius : mesh->bounds.max.z;
  radius *= getScale();

  if(dist <= radius)
  {
    *depth = toModelLength;
    return true;
  }
  
  // No hit
  return false;
}