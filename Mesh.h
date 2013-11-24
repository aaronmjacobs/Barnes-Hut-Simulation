#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"

#include "GLBridge.h"

#include "NewMeshParser/BasicModel.h"
#include <vector>
#include <map>

typedef struct
{
  glm::vec3 min, max;
} Bounds;

class Mesh
{
private:
  Mesh(const char* filePath, bool scaleOnLoad);
  static std::map<const char*, Mesh*> meshMap;

public: // TODO
  GLuint vertexBuffObj, indexBuffObj, normalBuffObj;
  unsigned int indexCount;
  Bounds bounds;

public:
  static Mesh* load(const char* filePath, bool scaleOnLoad);
};

#endif