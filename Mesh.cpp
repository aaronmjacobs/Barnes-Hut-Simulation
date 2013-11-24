#include "Mesh.h"

std::map<const char*, Mesh*> Mesh::meshMap;

Mesh* Mesh::load(const char* filePath, bool scaleOnLoad)
{
  // If the model has already been loaded once, just return a reference to it,
  // otherwise load it.
  std::map<const char*, Mesh*>::iterator it = meshMap.find(filePath);
  if(it != meshMap.end())
  {
    return it->second;
  }
  else
  {
    meshMap[filePath] = new Mesh(filePath, scaleOnLoad);
    return meshMap[filePath];
  }
}

Mesh::Mesh(const char* filePath, bool scaleOnLoad)
{
  // Parse the model file
  BasicModel model(filePath);
  indexCount = model.Triangles.size();
  
  bounds.min = glm::vec3(model.min_x, model.min_y, model.min_z);
  bounds.max = glm::vec3(model.max_x, model.max_y, model.max_z);
  
  // TODO Remove translations (and scale?)

  // Calculate x and y translations required to center the model
  float xTranslate = -(model.max_x + model.min_x) / 2.0f;
  float yTranslate = -(model.max_y + model.min_y) / 2.0f;

  // Calculate extrema values to scale properly
  float initialScale = 1.0f;
  if(scaleOnLoad)
  {
    float xExtrema = fabs(model.max_x) > fabs(model.min_x) ? fabs(model.max_x) : fabs(model.min_x);
    float yExtrema = fabs(model.max_y) > fabs(model.min_y) ? fabs(model.max_y) : fabs(model.min_y);
    float zExtrema = fabs(model.max_z) > fabs(model.min_z) ? fabs(model.max_z) : fabs(model.min_z);
    float extrema = xExtrema > yExtrema ? xExtrema : yExtrema;
    extrema = extrema > zExtrema ? extrema : zExtrema;
    if(extrema == 0.0f)
    {
      extrema = 1.0f;
    }
    initialScale = 1.0f / extrema;
    
    bounds.min.x += xTranslate;
    bounds.max.x += xTranslate;
    bounds.min.y += yTranslate;
    bounds.max.y += yTranslate;
    
    bounds.min *= initialScale;
    bounds.max *= initialScale;
  }

  float* vertices = new float[model.Vertices.size() * 3];
  unsigned int* indices = new unsigned int[indexCount * 3];
  glm::vec3* vertNormals = new glm::vec3[model.Vertices.size()];
  float* normals = new float[model.Vertices.size() * 3];

  // Build the vertex array
  for(int i = 0, j = 0; i < model.Vertices.size(); ++i, j += 3)
  {
    vertices[j] = (model.Vertices[i]->x + xTranslate) * initialScale;
    vertices[j + 1] = (model.Vertices[i]->y + yTranslate) * initialScale;
    vertices[j + 2] = (model.Vertices[i]->z) * initialScale;
  }

  // Build the index array, and calculate / sum up the normals
  for(int i = 0, j = 0; i < indexCount; ++i, j += 3)
  {
    indices[j] = model.Triangles[i]->v1 - 1;
    indices[j + 1] = model.Triangles[i]->v2 - 1;
    indices[j + 2] = model.Triangles[i]->v3 - 1;

    glm::vec3 w(vertices[indices[j] * 3],
                vertices[indices[j] * 3 + 1],
                vertices[indices[j] * 3 + 2]);
    glm::vec3 u(vertices[indices[j + 1] * 3],
                vertices[indices[j + 1] * 3 + 1],
                vertices[indices[j + 1] * 3 + 2]);
    glm::vec3 v(vertices[indices[j + 2] * 3],
                vertices[indices[j + 2] * 3 + 1],
                vertices[indices[j + 2] * 3 + 2]);
    u = glm::normalize(u - w);
    v = glm::normalize(v - w);
    glm::vec3 faceNormal = glm::normalize(glm::cross(u, v));

    vertNormals[indices[j]] += faceNormal;
    vertNormals[indices[j + 1]] += faceNormal;
    vertNormals[indices[j + 2]] += faceNormal;
  }

  // Build the normal array
  for(int i = 0, j = 0; i < model.Vertices.size(); ++i, j += 3)
  {
    vertNormals[i] = glm::normalize(vertNormals[i]);

    normals[j] = vertNormals[i].x;
    normals[j + 1] = vertNormals[i].y;
    normals[j + 2] = vertNormals[i].z;
  }

  glGenBuffers(1, &vertexBuffObj);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffObj);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(float) * model.Vertices.size() * 3,
               vertices,
               GL_STATIC_DRAW);

  glGenBuffers(1, &indexBuffObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * indexCount * 3,
               indices,
               GL_STATIC_DRAW);

  glGenBuffers(1, &normalBuffObj);
  glBindBuffer(GL_ARRAY_BUFFER, normalBuffObj);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(float) * model.Vertices.size() * 3,
               normals,
               GL_STATIC_DRAW);

  delete[] vertices;
  delete[] indices;
  delete[] vertNormals;
  delete[] normals;
}