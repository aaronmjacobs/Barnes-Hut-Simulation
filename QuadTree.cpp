//#define NDEBUG
#include <cassert>

#include <pthread.h>

#include "QuadTree.h"
#include "Particle.h"
#include "Model.h"

#include "GLBridge.h"

#define TIMESTEP 0.005f

struct IndexInfo {
  QuadTree* tree;
  Particle* particleArray;
  size_t startIndex, endIndex;
};

void* integrateRange(void* info) {
  IndexInfo* indexInfo = (IndexInfo*)info;
  for (size_t i = indexInfo->startIndex; i < indexInfo->endIndex; ++i) {
    indexInfo->tree->integrate(&(indexInfo->particleArray[i]));
  }
  return NULL;
}

void QuadTree::integrate(Particle* particle) {
  particle->force = glm::vec2();
  calcForces(particle);
  
  particle->pos += particle->vel * TIMESTEP
                + 0.5f * particle->acc * TIMESTEP * TIMESTEP;
              
  particle->vel += particle->acc * TIMESTEP;
  
  particle->acc = particle->force / particle->getMass();
}

//#define MEASURE_SPEED

QuadTree::QuadTree(Particle* particleArray, size_t numParticles, float bigG, bool pause) {
#ifdef MEASURE_SPEED
  int atStart = glutGet(GLUT_ELAPSED_TIME);
#endif
  
  this->particleArray = particleArray;
  this->numParticles = numParticles;
  this->bigG = bigG;
  
  assert(numParticles > 0);
  float minX = particleArray[0].pos.x,
        minY = particleArray[0].pos.y,
        maxX = particleArray[0].pos.x,
        maxY = particleArray[0].pos.y;
  for (size_t i = 0; i < numParticles; ++i) {
    if (particleArray[i].pos.x < minX) {
      minX = particleArray[i].pos.x;
    }
    if (particleArray[i].pos.y < minY) {
      minY = particleArray[i].pos.y;
    }
    if (particleArray[i].pos.x > maxX) {
      maxX = particleArray[i].pos.x;
    }
    if (particleArray[i].pos.y > maxY) {
      maxY = particleArray[i].pos.y;
    }
  }
  
  minCoords = glm::vec2(minX, minY);
  maxCoords = glm::vec2(maxX, maxY);
  midCoords = glm::vec2((minCoords.x + maxCoords.x) / 2, (minCoords.y + maxCoords.y) / 2);
  
  hasChildren = false;
  nw = ne = sw = se = NULL;
  particles = std::list<Particle*>();
  centerOfMass = glm::vec2();
  mass = 0.0f;
  
  const int NUM_THREADS = 8;
  pthread_t threads[NUM_THREADS];
  
#ifdef MEASURE_SPEED
  int pre = glutGet(GLUT_ELAPSED_TIME);
#endif
  // Build tree
  for (size_t i = 0; i < numParticles; ++i) {
    insert(particleArray + i);
  }
#ifdef MEASURE_SPEED
  int post = glutGet(GLUT_ELAPSED_TIME);
  printf("Time for all insertions: %d\n", post - pre);
#endif

  // Calc centers of mass
  calcCenterOfMass();
  
#ifdef MEASURE_SPEED
  int start = glutGet(GLUT_ELAPSED_TIME);
  printf("Time for center of mass calculations: %d\n", start - post);
#endif
  
  if (!pause) {
    // Calculate forces
    size_t particlesPerThread = numParticles / NUM_THREADS;
    IndexInfo* threadInfo = new IndexInfo[NUM_THREADS];
    for (size_t t = 0; t < NUM_THREADS; ++t) {
      threadInfo[t].tree = this;
      threadInfo[t].particleArray = particleArray;
      threadInfo[t].startIndex = t * particlesPerThread;
      if (t < NUM_THREADS - 1) {
        threadInfo[t].endIndex = (t + 1) * particlesPerThread;
      } else {
        // Make sure any leftover particles are taken care of by the last thread
        // (in case numParticles is not evenly divisible by NUM_THREADS)
        threadInfo[t].endIndex = numParticles;
      }
      
      pthread_create(&threads[t], NULL, integrateRange, (void *)(threadInfo + t));
    }
    
    for (size_t t = 0; t < NUM_THREADS; ++t) {
      pthread_join(threads[t], NULL);
    }
  }
  
  /*for (size_t i = 0; i < numParticles; ++i) {
    integrate(&particleArray[i]);
  }*/
  
#ifdef MEASURE_SPEED
  int end = glutGet(GLUT_ELAPSED_TIME);
  printf("%Time for force calculations: %d\n", end - start);

  int preDraw = glutGet(GLUT_ELAPSED_TIME);
#endif
  
  
  if (Particle::model) {
    Particle::model->preDraw();
  }

  for (size_t i = 0; i < numParticles; ++i) {
    //integrate(&particleArray[i]);
    
    // Draw
    particleArray[i].draw();
  }
  if (Particle::model) {
    Particle::model->postDraw();
  }
#ifdef MEASURE_SPEED
  int postDraw = glutGet(GLUT_ELAPSED_TIME);
  printf("Time for drawing: %d\n", postDraw - preDraw);
  printf("Total: %d\n\n", glutGet(GLUT_ELAPSED_TIME) - atStart);
#endif
}

QuadTree::QuadTree(glm::vec2 minCoords, glm::vec2 maxCoords, float bigG) {
  this->minCoords = minCoords;
  this->maxCoords = maxCoords;
  this->bigG = bigG;
  midCoords = glm::vec2((minCoords.x + maxCoords.x) / 2, (minCoords.y + maxCoords.y) / 2);
  
  hasChildren = false;
  nw = ne = sw = se = NULL;
  particles = std::list<Particle*>();
  centerOfMass = glm::vec2();
  mass = 0.0f;
}

QuadTree::~QuadTree() {
  if (nw) {
    delete nw;
    nw = NULL;
  }
  if (ne) {
    delete ne;
    ne = NULL;
  }
  if (sw) {
    delete sw;
    sw = NULL;
  }
  if (se) {
    delete se;
    se = NULL;
  }
}

#define THETA 0.7f

void QuadTree::calcForces(Particle* particle) {
  if (hasChildren) {
    if (nw) {
      calcForceFromChild(nw, particle);
    }
    if (ne) {
      calcForceFromChild(ne, particle);
    }
    if (sw) {
      calcForceFromChild(sw, particle);
    }
    if (se) {
      calcForceFromChild(se, particle);
    }
  } else {
    std::list<Particle*>::const_iterator iterator = particles.begin(),
                                         end = particles.end();
    while (iterator != end) {
      if (particle != *iterator) {
        particle->force += calcForce(particle, *iterator);
      }
      ++iterator;
    }
  }
}

void QuadTree::calcForceFromChild(QuadTree* child, Particle* particle) {
  glm::vec2 toChild = child->centerOfMass - particle->pos;
  float r = glm::length(toChild);
  float D = child->size();
  
  if (D / r < THETA) {
    particle->force += calcForce(particle, child->centerOfMass, child->mass);
  } else {
    child->calcForces(particle);
  }
}

glm::vec2 QuadTree::calcForce(Particle* particle, Particle* other) {
  return calcForce(particle, other->pos, other->mass);
}

#define MIN_DISTANCE 1.0f

glm::vec2 QuadTree::calcForce(Particle* particle, glm::vec2 otherPos, float otherMass) {
  glm::vec2 toOther = otherPos - particle->pos;
  float r = glm::length(toOther);
  if (r < MIN_DISTANCE) {
    r = MIN_DISTANCE;
  }
  return bigG * particle->mass * otherMass * (toOther / (r * r * r));
}

void QuadTree::calcCenterOfMassFromChild(QuadTree* child) {
  if (child == nw) {
    nw->calcCenterOfMass();
    mass += nw->mass;
    centerOfMass += nw->mass * nw->centerOfMass;
  } else if (child == ne) {
    ne->calcCenterOfMass();
    mass += ne->mass;
    centerOfMass += ne->mass * ne->centerOfMass;
  } else if (child == sw) {
    sw->calcCenterOfMass();
    mass += sw->mass;
    centerOfMass += sw->mass * sw->centerOfMass;
  } else if (child == se) {
    se->calcCenterOfMass();
    mass += se->mass;
    centerOfMass += se->mass * se->centerOfMass;
  }
}

void QuadTree::calcCenterOfMass() {
  // TODO needed?
  //mass = 0.0f;
  //centerOfMass = glm::vec2();

  if (hasChildren) {
    if (nw) {
      calcCenterOfMassFromChild(nw);
    }
    if (ne) {
      calcCenterOfMassFromChild(ne);
    }
    if (sw) {
      calcCenterOfMassFromChild(sw);
    }
    if (se) {
      calcCenterOfMassFromChild(se);
    }
  } else {
    std::list<Particle*>::const_iterator iterator = particles.begin(),
                                       end = particles.end();
    while (iterator != end) {
      mass += (*iterator)->mass;
      centerOfMass += (*iterator)->mass * (*iterator)->pos;
      ++iterator;
    }
  }
  
  assert(mass != 0.0f);
  centerOfMass /= mass;
}

void QuadTree::insert(Particle* particle) {
  if (particles.size() == PARTICLES_PER_NODE) {
      split();
  }
  if (hasChildren) {
    insertIntoChild(particle);
  } else {
    particles.push_back(particle);
  }
}

void QuadTree::split() {
  hasChildren = true;
  
  std::list<Particle*>::const_iterator iterator = particles.begin(),
                                       end = particles.end();
  while (iterator != end) {
    insertIntoChild(*iterator);
    iterator = particles.erase(iterator);
  }
}

/*void QuadTree::merge() {
  assert(hasChildren);
  hasChildren = false;
  
  if (nw) {
    copyParticlesFrom(nw);
    delete nw;
    nw = NULL;
  }
  if (ne) {
    copyParticlesFrom(ne);
    delete ne;
    ne = NULL;
  }
  if (sw) {
    copyParticlesFrom(sw);
    delete sw;
    sw = NULL;
  }
  if (se) {
    copyParticlesFrom(se);
    delete se;
    se = NULL;
  }
  assert(!hasChildren);
}*/

/*void QuadTree::copyParticlesFrom(QuadTree* leaf) {
  particles.insert(particles.end(), leaf->particles.begin(), leaf->particles.end());
}*/

void QuadTree::insertIntoChild(Particle* particle) {
  if (particle->pos.x < midCoords.x) {
    if (particle->pos.y < midCoords.y) {
      if (!sw) {
        initializeChild(&sw);
      }
      sw->insert(particle);
    } else {
      if (!nw) {
        initializeChild(&nw);
      }
      nw->insert(particle);
    }
  } else {
    if (particle->pos.y < midCoords.y) {
      if (!se) {
        initializeChild(&se);
      }
      se->insert(particle);
    } else {
      if (!ne) {
        initializeChild(&ne);
      }
      ne->insert(particle);
    }
  }
}

void QuadTree::initializeChild(QuadTree** child) {
  if (child == &nw) {
    nw = new QuadTree(glm::vec2(minCoords.x, midCoords.y), glm::vec2(midCoords.x, maxCoords.y), bigG);
  } else if (child == &ne) {
    ne = new QuadTree(midCoords, maxCoords, bigG);
  } else if (child == &sw) {
    sw = new QuadTree(minCoords, midCoords, bigG);
  } else if (child == &se) {
    se = new QuadTree(glm::vec2(midCoords.x, minCoords.y), glm::vec2(maxCoords.x, midCoords.y), bigG);
  } else {
    assert(false);
  }
}