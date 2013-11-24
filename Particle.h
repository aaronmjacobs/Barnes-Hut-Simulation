#ifndef PARTICLE_H
#define PARTICLE_H

#include "glm/glm.hpp"

class Model;

#define DEFAULT_MASS 1.0f

class Particle {
private:
  bool relocate;
public:
  static Model* model;
  glm::vec2 pos, vel, acc, force;
  float mass;

  Particle()
    : mass(DEFAULT_MASS), relocate(false) {
    pos = vel = acc = force = glm::vec2();
  }
  Particle(glm::vec2 pos, float mass = DEFAULT_MASS) {
    this->pos = pos;
    this->mass = mass;
    vel = acc = force = glm::vec2();
    this->relocate = false;
  }
  float getMass() {
    return mass;
  }
  float setMass(float mass) {
    this->mass = mass;
  }
  bool needsRelocation() {
    return relocate;
  }
  bool setNeedsRelocation(bool relocate) {
    this->relocate = relocate;
  }
  void draw();
};

#endif