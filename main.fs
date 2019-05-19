#version 440

layout(origin_upper_left) in vec4 gl_FragCoord;

layout(std430, binding = 3) buffer b1
{
  float data[];
} ssbo;

uniform ivec2 winsize;
uniform int tick;
uniform int pixcount;

out vec4 color;

void main() {
  int id = int(gl_FragCoord.y * winsize.x + gl_FragCoord.x);
  color.rgb = ssbo.data[(1 - tick)*pixcount + id] * vec3(1, 1, 1);
  color.a = 1.0f;
}
