#ifndef QUADTREE_H
#define QUADTREE_H

#include <list>
#include "glm/glm.hpp"

#define PARTICLES_PER_NODE 1

class Particle;

class QuadTree {
private:
  Particle* particleArray;
  size_t numParticles;

  QuadTree *nw, *ne, *sw, *se;
  bool hasChildren;
  std::list<Particle*> particles;
  glm::vec2 minCoords, maxCoords, midCoords, centerOfMass;
  float mass, bigG;

  QuadTree(glm::vec2 minCoords, glm::vec2 maxCoords, float bigG);
  
  void split();
  //void merge();
  void calcCenterOfMass();
  void insertIntoChild(Particle* particle);
  void initializeChild(QuadTree** child);
  //void copyParticlesFrom(QuadTree* leaf);
  
  void calcForces(Particle* particle);
  glm::vec2 calcForce(Particle* particle, Particle* other);
  glm::vec2 calcForce(Particle* particle, glm::vec2 otherPos, float otherMass);
  void calcForceFromChild(QuadTree* child, Particle* particle);
  void calcCenterOfMassFromChild(QuadTree* child);
  
  float size() {
    return (maxCoords.x - minCoords.x) * (maxCoords.y - minCoords.y);
  }

public:
  QuadTree(Particle* particleArray, size_t numParticles, float bigG, bool pause);
  ~QuadTree();
  void insert(Particle* particle);
  void remove(Particle* particle);
  
  void integrate(Particle* particle);
};

#endif