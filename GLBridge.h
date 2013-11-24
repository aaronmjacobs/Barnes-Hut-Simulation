#ifndef GL_BRIDGE_H
#define GL_BRIDGE_H

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif

#ifdef __unix__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/glut.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut.lib")
#endif

#include "glm/glm.hpp"

// TODO remove
#include "GLSL_helper.h"

typedef struct
{
  glm::vec3 ambient, diffuse, specular, emission;
  float shininess;
} Material;

typedef struct
{
  GLint uModelMatrix, uNormalMatrix, aPosition, aNormal;
} TransHandles;

typedef struct
{
  GLint uAmbientColor, uDiffuseColor, uSpecularColor, uEmissionColor, uShininess;
  
  GLint uColor;
} MaterialHandles;

typedef struct
{
  GLint uLightPosition, uLightColor, uLightConstFalloff, uLightLinearFalloff, uLightSquareFalloff;
} LightHandles;

#define MAX_LIGHTS 10

class GLBridge
{
private:
  static TransHandles transHandles;
  static MaterialHandles materialHandles;

  static GLint h_uViewMatrix;
  static GLint h_uProjMatrix;
  static GLint h_uCameraPos;
  
  static int shaderProgram;
  
  static GLint numLights;
  static LightHandles lightHandlesArray[MAX_LIGHTS];
  static int lightHandlesInUse;

public:
  static int InstallShader(const GLchar *vShaderName, const GLchar *fShaderName);

  static TransHandles getTransHandles();
  static MaterialHandles getMaterialHandles();
  static LightHandles getLightHandles();
  static void onDraw();
  static GLint getUViewMatrix();
  static GLint getUProjMatrix();
  static GLint getUCameraPos();
  static int getShaderProgram();
  static GLint getNumLightsHandle();
};

#endif