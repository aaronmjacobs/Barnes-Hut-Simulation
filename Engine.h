#ifndef ENGINE_H
#define ENGINE_H

#include "GLBridge.h"
#include "Scene.h"

#define FPS 60
#define FRAME_DELAY (1000 / FPS)

class Engine {
private:
  static Engine* engine;
  
  Scene* scene;

  int windowWidth, windowHeight;

  // Physics timer
  int currentTime;
  double t, accumulator;
  static const double dt = 1.0 / FPS;
  
  int lastMouseX, lastMouseY, lastButton;
  bool walkingForward, walkingBackward, walkingLeft, walkingRight;

  glm::mat4 projection;

  static void onDrawStatic();
  static void onKeyDownStatic(unsigned char key, int x, int y);
  static void onKeyUpStatic(unsigned char key, int x, int y);
  static void onMouseClickStatic(int button, int state, int x, int y);
  static void onMouseMoveStatic(int x, int y);
  static void onReshapeStatic(int width, int height);
  static void loopStatic(int value);

  void onDraw();
  void onKeyDown(unsigned char key, int x, int y);
  void onKeyUp(unsigned char key, int x, int y);
  void onMouseClick(int button, int state, int x, int y);
  void onMouseMove(int x, int y);
  void onReshape(int width, int height);

  void loop(int value);

public:
  Engine(int argc, char *argv[]);
  void init(int width, int height, const char* title);
  void start();
  void setScene(Scene* scene);
  Scene* getScene();
};

#endif