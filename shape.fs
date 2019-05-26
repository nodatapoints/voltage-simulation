#version 440

layout(origin_upper_left) in vec4 gl_FragCoord;

layout(std430, binding=3) buffer b1 {
    float data[];
};

layout(std430, binding=4) buffer b2 {
    bool isStatic[];
};

in float potential;

uniform ivec2 winsize;

void main() {
	int id = int(gl_FragCoord.y*winsize.x + gl_FragCoord.x);
	data[id] = potential;
    isStatic[id] = true;
}
