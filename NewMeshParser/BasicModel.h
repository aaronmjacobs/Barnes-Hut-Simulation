#if !defined BASIC_MODEL_H
#define BASIC_MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include "LoadedModel.h"

class BasicModel : virtual public LoadedModel
{
public:
   BasicModel(std::string filename);
   ~BasicModel();

   //stl vector to store all the triangles in the mesh
   std::vector<Tri *> Triangles;
   //stl vector to store all the vertices in the mesh
   std::vector<Vector3 *> Vertices;
   //stl vector to store all the vertices normals in the mesh
   std::vector<Vector3 *> VerticesNormals;

   void draw(float,float,float);
   void setLOD(int);

protected:
#if defined(GL)
   GLuint id;
#endif

   Tri* parseTri(std::string line);
   void ReadFile(std::string filename);
   Tri* parseTriObj(std::string line);
   void ReadFileObj(std::string filename);
#if defined(GL)
   GLuint createDL();
#endif
   void drawTria(Tri* t);
  
};

#endif