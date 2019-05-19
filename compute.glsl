#version 440

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 3) buffer b1
{
  float data[];
} ssbo;

uniform ivec2 comp_winsize;
uniform int comp_pixcount, comp_tick;
uniform float comp_alpha;

void main() {
  const int w = comp_winsize.x;
  const int id = int(gl_GlobalInvocationID.x);

  const int base = id + comp_tick * comp_pixcount;
  const int pbase = id + (1 - comp_tick) * comp_pixcount;

  const float average = .25*(ssbo.data[base - 1] +      //  l
                       ssbo.data[base + 1] +      //  r
                       ssbo.data[base - w] +      // u
                       ssbo.data[base + w]);

  ssbo.data[pbase] = comp_alpha*ssbo.data[pbase] + (1-comp_alpha)*average;
}
