#version 120

uniform vec3 uColor;

void main() {
  gl_FragColor = vec4(uColor.r, uColor.g, uColor.b, 1.0);
}