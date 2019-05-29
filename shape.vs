#version 440 core

layout(location=1) in vec2 vP;
layout(location=2) in float vPotential;

out float potential;
const float scale = 10;

void main() {
	gl_Position = vec4(vP.xy/scale, 0, 1);
    potential = vPotential;
}


