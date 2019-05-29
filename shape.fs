/* This fragment shader initializes the potential and isStatic buffers
 * from the fragments of the shapes.
 * 
 * After passing through the rasterizer, the abstract shapes defined in
 * the vertex arrays are now converted into a grid of fragments
 * (abstract, yet to be pixels).
 * 
 * When this shader is invoked, it means that current fragment lies
 * within one of the specified shapes. In that case, the potential is
 * set and the pixel is marked as static.
 *
 * The whole thing is written to the front buffer, so no offset
 */
#version 440

layout(origin_upper_left) in vec4 gl_FragCoord;

layout(std430, binding=3) buffer b1 {
    writeonly float data[];
};

layout(std430, binding=4) buffer b2 {
    writeonly bool isStatic[];
};

in float potential;
uniform ivec2 windowSize;

void main() {
    const int id = int(gl_FragCoord.y*windowSize.x + gl_FragCoord.x);
    data[id] = potential;
    isStatic[id] = true;
}
