#version 440

layout(local_size_x=128, local_size_y=1, local_size_z=1) in;

layout(std430, binding=3) buffer b1 {
    float data[];
};

layout(std430, binding=4) buffer b2 {
    readonly bool isStatic[];
};

uniform ivec2 comp_winsize;
uniform int comp_pixcount, comp_tick;
uniform float comp_alpha;

void main() {
    const int w = comp_winsize.x;
    const int id = int(gl_GlobalInvocationID.x);

    const int base = id + comp_tick*comp_pixcount;
    const int pbase = id + (1-comp_tick)*comp_pixcount;

    const float average = .25*(
          data[base-1]
        + data[base+1]
        + data[base-w]
        + data[base+w]
    );

    if (isStatic[id]) 
        data[pbase] = data[base];
    else
        data[pbase] = comp_alpha*data[base] + (1-comp_alpha)*average;
}
