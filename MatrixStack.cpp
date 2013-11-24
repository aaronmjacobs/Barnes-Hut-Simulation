#include "MatrixStack.h"

MatrixStack::MatrixStack() {
  currentMatrix = glm::mat4(1.0f);
}

void MatrixStack::push() {
  matrixStack.push(currentMatrix);
}

void MatrixStack::pop() {
  assert(!matrixStack.empty());
  
  currentMatrix = matrixStack.top();
  matrixStack.pop();
}

glm::mat4* MatrixStack::getMatrix() {
  return &currentMatrix;
}