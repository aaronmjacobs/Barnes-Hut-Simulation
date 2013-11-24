#version 120

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

attribute vec3 aPosition;

void main() {
  gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition.x, aPosition.y, aPosition.z, 1);
}