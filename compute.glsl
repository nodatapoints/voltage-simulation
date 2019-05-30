/* This is the actual program implementing the algorithm
 * This compute shader updates the potential buffer (see main.cpp)
 * taking in account the static sources.
 */
#version 440

// set up workgroups in 1D
layout(local_size_x=128, local_size_y=1, local_size_z=1) in;


layout(std430, binding=3) buffer b1 {
    float potential[];
};

layout(std430, binding=4) buffer b2 {
    readonly bool isStatic[];
};

uniform ivec2 windowSize;
uniform int nPixels, tick;
uniform float alpha;

void main() {
    const int w = windowSize.x;

    // id is the index of the position the current invocation is
    // assigned to. It ranges from 0 to nPixels-1 to cover the screen.
    // 
    // -- or at least that's how it's supposed to be.
    // Actually it starts at w/2 and I have no idea why. But it works, so
    // I just roll with it and pretend like it's fine.
    const int id = int(gl_GlobalInvocationID.x);

    // base is index of the original pixel, while pbase is the
    // pixel to be updated.  When tick is 0, base has an offset
    // of zero, and is therefore located in the front buffer, while
    // pbase has an offset of nPixels, pointing to the corresponding
    // pixel in the back buffer.
    // When tick is 1, the roles are changed.
    const int base = id + tick*nPixels;
    const int pbase = id + (1-tick)*nPixels;

    const float average = .25*(
          potential[base-1]  // left
        + potential[base+1]  // right
        + potential[base-w]  // above
        + potential[base+w]  // under
    );

    // when it's static or the left or top edge of the screen
    // bottom and right are not needed since it just wraps around with
    // the 1D layout
    if (isStatic[id] || (id % w == w/2) || (id < w*3/2))
        potential[pbase] = potential[base];
    else
        potential[pbase] = alpha*potential[base] + (1-alpha)*average;
}
