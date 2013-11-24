#include <sstream>
#include "GLBridge.h"

TransHandles GLBridge::transHandles;
MaterialHandles GLBridge::materialHandles;
GLint GLBridge::h_uViewMatrix;
GLint GLBridge::h_uProjMatrix;
GLint GLBridge::h_uCameraPos;
int GLBridge::shaderProgram;

GLint GLBridge::numLights;
LightHandles GLBridge::lightHandlesArray[MAX_LIGHTS];
int GLBridge::lightHandlesInUse;

int GLBridge::InstallShader(const GLchar *vShaderName, const GLchar *fShaderName) {
  GLuint VS; //handles to shader object
  GLuint FS;
  GLint vCompiled, linked; //status of shader
  GLint vCompiledF, linkedF; //status of shader

  VS = glCreateShader(GL_VERTEX_SHADER);
  FS = glCreateShader(GL_FRAGMENT_SHADER);

  //load the source
  glShaderSource(VS, 1, &vShaderName, NULL);

  //compile shader and print log
  glCompileShader(VS);
  /* check shader status requires helper functions */
  printOpenGLError();
  glGetShaderiv(VS, GL_COMPILE_STATUS, &vCompiled);
  printShaderInfoLog(VS);

  if (!vCompiled) {
      printf("Error compiling the shader %s", vShaderName);
      return 0;
  }

/***************************************/

  //load the source
  glShaderSource(FS, 1, &fShaderName, NULL);

  //compile shader and print log
  glCompileShader(FS);
  /* check shader status requires helper functions */
  printOpenGLError();
  glGetShaderiv(FS, GL_COMPILE_STATUS, &vCompiledF);
  printShaderInfoLog(FS);

  if (!vCompiled) {
      printf("Error compiling the shader %s", fShaderName);
      return 0;
  }

/**************************************/

  //create a program object and attach the compiled shader
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, VS);
  glAttachShader(shaderProgram, FS);
  
  //glBindFragDataLocationEXT(shaderProgram, 0, "fragColor");

  glLinkProgram(shaderProgram);
  /* check shader status requires helper functions */
  printOpenGLError();
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
  printProgramInfoLog(shaderProgram);

  glUseProgram(shaderProgram);

  /* get handles to attribute data */
  transHandles.aPosition = safe_glGetAttribLocation(shaderProgram, "aPosition");
  transHandles.uModelMatrix = safe_glGetUniformLocation(shaderProgram, "uModelMatrix");
  h_uProjMatrix = safe_glGetUniformLocation(shaderProgram, "uProjMatrix");
  h_uViewMatrix = safe_glGetUniformLocation(shaderProgram, "uViewMatrix");
  
  materialHandles.uColor = safe_glGetUniformLocation(shaderProgram, "uColor");

  printf("sucessfully installed shader %d\n", shaderProgram);
  return 1;
}

GLint GLBridge::getNumLightsHandle() {
  return numLights;
}

TransHandles GLBridge::getTransHandles() {
  return transHandles;
}

MaterialHandles GLBridge::getMaterialHandles() {
  return materialHandles;
}

LightHandles GLBridge::getLightHandles() {
  if(lightHandlesInUse < MAX_LIGHTS) {
    return lightHandlesArray[lightHandlesInUse++];
  }
  
  // TODO
  return LightHandles();
}

void GLBridge::onDraw() {
  lightHandlesInUse = 0;
}

GLint GLBridge::getUViewMatrix() {
  return h_uViewMatrix;
}

GLint GLBridge::getUProjMatrix() {
  return h_uProjMatrix;
}

GLint GLBridge::getUCameraPos() {
  return h_uCameraPos;
}

int GLBridge::getShaderProgram() {
  return shaderProgram;
}