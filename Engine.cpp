#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Engine.h"
#include "FileIO.h"
#include "QuadTree.h"
#include "Particle.h"

#define WALK_SPEED 0.01f
#define ROTATE_SCALE 0.005f

namespace Config {
bool drawControlPoints = false;
}

Engine* Engine::engine = NULL;

Particle* particles;
size_t numParticles;
bool pause;

float range, velocity, bigG;

size_t nextNumParticles;
float nextRange;
float nextVelocity;
float nextBigG;

Particle genParticle() {
  // Galaxy-styled simulation:
  // RANGE = 2.0f
  // VEL = 10.0f
  // BIG_G = 0.1f
  
  float x = (((float)rand() / RAND_MAX) - 0.5f);
  float y = (((float)rand() / RAND_MAX) - 0.5f);
  glm::vec2 pos = glm::normalize(glm::vec2(x, y)) * range * ((float)rand() / RAND_MAX);
  
  glm::vec3 toParticle(pos.x, pos.y, 0.0f);
  glm::vec3 in(0.0f, 0.0f, -1.0f);
  glm::vec3 front = glm::cross(toParticle, in);
  
  Particle particle(pos);
  float dist = glm::length(pos);
  particle.vel = glm::normalize(glm::vec2(front.x, front.y)) * (float)sqrt(sqrt(dist)) * velocity;
  return particle;
}

void initParticles() {
  numParticles = nextNumParticles;
  range = nextRange;
  velocity = nextVelocity;
  bigG = nextBigG;
  
  printf("Running simulation:\nParticles: %d\nRange: %.2f\nVelocity: %.2f\nGravity: %.4f\n\n", numParticles, range, velocity, bigG);
  particles = new Particle[numParticles];
  for (size_t i = 0; i < numParticles; ++i) {
    particles[i] = genParticle();
  }
}

void Engine::onDrawStatic() {
  if (engine) {
    engine->onDraw();
  }
}

void Engine::onKeyDownStatic(unsigned char key, int x, int y) {
  if (engine) {
    engine->onKeyDown(key, x, y);
  }
}

void Engine::onKeyUpStatic(unsigned char key, int x, int y) {
  if (engine) {
    engine->onKeyUp(key, x, y);
  }
}

void Engine::onMouseClickStatic(int button, int state, int x, int y) {
  if (engine) {
    engine->onMouseClick(button, state, x, y);
  }
}

void Engine::onMouseMoveStatic(int x, int y) {
  if (engine) {
    engine->onMouseMove(x, y);
  }
}

void Engine::onReshapeStatic(int width, int height) {
  if (engine) {
    engine->onReshape(width, height);
  }
}

void Engine::loopStatic(int value) {
  if (engine) {
    engine->loop(value);
  }
}

Engine::Engine(int argc, char *argv[]) {
  if (engine) {
    std::cerr << "Only one instance of Engine allowed at a time!" << std::endl;
    exit(EXIT_FAILURE);
  }
  engine = this;
  scene = NULL;
  projection = glm::mat4();
  
  lastMouseX = lastMouseY = lastButton = 0;
  walkingForward = walkingBackward = walkingLeft = walkingRight = false;

  glutInit(&argc, argv);
}

void Engine::init(int width, int height, const char* title) {
  // GLUT initialization
  glutInitWindowSize(width, height);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutCreateWindow(title);
  
  // GLUT callbacks
  glutDisplayFunc(onDrawStatic);
  glutKeyboardFunc(onKeyDownStatic);
  glutKeyboardUpFunc(onKeyUpStatic);
  glutMouseFunc(onMouseClickStatic);
  glutMotionFunc(onMouseMoveStatic);
  glutReshapeFunc(onReshapeStatic);

  // GLEW (Windows only)
#ifdef _WIN32
  if (GLEW_OK != glewInit()) {
      std::cerr << "Error initializing glew! " << glewGetErrorString(err) << std::endl;
      exit(EXIT_FAILURE);
  }
#endif

  srand(time(NULL));

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

  // Depth Buffer Setup
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  // TODO Is it ok to call glViewport?
  onReshape(width, height);
  
  // Load the shaders
  GLBridge::InstallShader(FileIO::readTextFile("phong.vert"), FileIO::readTextFile("phong.frag"));
  
  // Herp derp
  printf("\n");
  pause = false;
  nextNumParticles = 3000;
  nextRange = 2.0f;
  nextVelocity = 10.0f;
  nextBigG = 0.1f;
  initParticles();
}

void Engine::start() {
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  loop(FRAME_DELAY);
  glutMainLoop();
}

void Engine::onDraw() {
  // Clear
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Start our shader
  glUseProgram(GLBridge::getShaderProgram());

  // Projection
  glUniformMatrix4fv(GLBridge::getUProjMatrix(), 1, GL_FALSE, glm::value_ptr(projection));

  if (scene) {
    // Lights
    glUniform1i(GLBridge::getNumLightsHandle(), scene->getNumLights());
    GLBridge::onDraw(); // Resets light handles
    
    // Models
    scene->draw();
    
    // Derp
    QuadTree tree(particles, numParticles, bigG, pause);
  }

  // Disable the shader
  glUseProgram(0);

  glutSwapBuffers();
}

void Engine::onKeyDown(unsigned char key, int x, int y) {
  switch(key) {
  case 'w':
    walkingForward = true;
    break;
  case 's':
    walkingBackward = true;
    break;
  case 'a':
    walkingLeft = true;
    break;
  case 'd':
    walkingRight = true;
    break;
  case 'c':
    if (scene && scene->getCamera()) {
      scene->getCamera()->resetView();
      scene->getCamera()->fly(-7.0f);
    }
    break;
  case '\r':
  case '\n':
    printf("Number of particles: ");
    scanf("%d", &nextNumParticles);
    printf("Range: ");
    scanf("%f", &nextRange);
    printf("Initial velocity: ");
    scanf("%f", &nextVelocity);
    printf("Force of gravity: ");
    scanf("%f", &nextBigG);
    printf("\n");
    
    initParticles();
    break;
  case 'r':
    initParticles();
    break;
  case ' ':
    pause = !pause;
    break;
  default:
    break;
  }
}

void Engine::onKeyUp(unsigned char key, int x, int y) {
  switch(key) {
  case 'w':
    walkingForward = false;
    break;
  case 's':
    walkingBackward = false;
    break;
  case 'a':
    walkingLeft = false;
    break;
  case 'd':
    walkingRight = false;
    break;
  default:
    break;
  }
}

void Engine::onMouseClick(int button, int state, int x, int y) {
  //
  
  lastMouseX = x;
  lastMouseY = y;
  lastButton = button;
}

void Engine::onMouseMove(int x, int y) {
  if(lastButton == GLUT_RIGHT_BUTTON) {
    float dTheta = (x - lastMouseX) * ROTATE_SCALE;
    float dPhi = -(y - lastMouseY) * ROTATE_SCALE;
    
    if (scene && scene->getCamera()) {
      scene->getCamera()->rotate(dPhi, dTheta);
    }
  }
  
  lastMouseX = x;
  lastMouseY = y;
}

void Engine::onReshape(int width, int height) {
  windowWidth = width;
  windowHeight = height;

  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  projection = glm::perspective(45.0f, (float)width / height, 0.1f, 100.f);
}

void Engine::loop(int value) {
  int now = glutGet(GLUT_ELAPSED_TIME);
  int frameTime = now - currentTime;
  if (frameTime > 250) {
    frameTime = 250; // Cap the frame time to avoid spiraling
  }
  currentTime = now;

  accumulator += (frameTime / 1000.0);

  while (accumulator >= dt) {
    //scene.step(t, dt);
    t += dt;
    accumulator -= dt;
  }

  // Render using an interpolated state in order to prevent any jittering caused
  // by the physics rate and frame rate not being in sync (note this is purely a
  // visual thing - the actual physics steps that are kept throughout the
  // simulation do not use the interpolated values).
  //alpha = accumulator / dt;

  if (scene && scene->getCamera()) {
    Camera* camera = scene->getCamera();

    if(walkingForward) {
      camera->fly(WALK_SPEED * frameTime);
    }
    if(walkingBackward) {
      camera->fly(-WALK_SPEED * frameTime);
    }
    if(walkingLeft) {
      camera->strafe(-WALK_SPEED * frameTime);
    }
    if(walkingRight) {
      camera->strafe(WALK_SPEED * frameTime);
    }
  }

  glutPostRedisplay(); // Draw

  glutTimerFunc(FRAME_DELAY, loopStatic, 0);
}

void Engine::setScene(Scene* scene) {
  this->scene = scene;
}

Scene* Engine::getScene() {
  return scene;
}