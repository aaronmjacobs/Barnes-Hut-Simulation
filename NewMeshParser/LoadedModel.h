#if !defined __LOADED_MODEL_H__
#define __LOADED_MODEL_H__

#if defined(GL)
#include <GL/glut.h>
#endif

#include "utils.h"

class LoadedModel {

protected:

   // inner class to store triangle faces
   // note that v1, v2, and v3 are indices into the vertex array
   class Tri {
      public:
         int v1;
         int v2;
         int v3;
         Vector3 normal;
         Vector3 color;

         // constructors
         Tri(int in_v1, int in_v2, int in_v3) :
            v1(in_v1), v2(in_v2), v3(in_v3), normal(0, 1, 0) {}

         Tri() : normal(0, 1, 0) {}
   };

public:
   float max_extent;
   float max_x;
   float max_y;
   float max_z;
   float min_x;
   float min_y;
   float min_z;

protected:
   Vector3 center;

   // Parses the coordinates from the line in the following format
   //    Vertex <(ignored)> <x> <y> <z>
   // returns *new* Vector3 (you delete)
   Vector3* parseCoords(const std::string line) const
   {

      int vi;
      float x, y, z;
      if (sscanf(line.c_str(), "Vertex %d %g %g %g", &vi, &x, &y, &z) != 4) {
         printf("error reading coords\n");
      }
      Vector3 * v = new Vector3(x,y,z);
      return v;
   }
   
      // Parses the coordinates from the line in the following format
   //    Vertex <(ignored)> <x> <y> <z>
   // returns *new* Vector3 (you delete)
   Vector3* parseCoordsObj(const std::string line) const
   {
      float x, y, z;
      if (sscanf(line.c_str(), "v %g %g %g", &x, &y, &z) != 3) {
         printf("error reading coords\n");
      }
      Vector3 * v = new Vector3(x,y,z);
      return v;
   }

   // returns the first float found in the string
   float parseFloat(const std::string line) const
   {

      float f;
      if (sscanf(line.c_str(), "%g", &f) != 1) {
         printf("error reading float\n");
      }
      return f;
   }
};

#endif
