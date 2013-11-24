#include "Particle.h"
#include "Model.h"

Model* Particle::model = NULL;

void Particle::draw() {
  if (model) {
    model->setPosition(glm::vec3(pos.x, pos.y, 0.0f));
    model->mainDraw();
  }
}