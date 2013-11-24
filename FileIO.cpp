#include "FileIO.h"
#include <stdio.h>
#include <stdlib.h>

namespace FileIO {

char *readTextFile(const char *fileName) {
  FILE *file;
  char *text = NULL;
  int count = 0;
  
  if (fileName) {
    file = fopen(fileName, "r");
    if (file) {
      fseek(file, 0, SEEK_END);
      count = ftell(file);
      rewind(file);
      
      if (count > 0) {
        text = (char*)malloc(sizeof(char) * (count + 1));
        count = fread(text, sizeof(char), count, file);
        text[count] = '\0';
      }
      
      fclose(file);
    }
  }
  
  return text;
}

} // namespace FileIO