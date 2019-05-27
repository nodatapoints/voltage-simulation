#version 440 core

layout(location=1) in vec2 vP;
layout(location=2) in float vPotential;

out float potential;

void main() {
	gl_Position = vec4(vP.xy, 0, 1);
    potential = vPotential;
}


