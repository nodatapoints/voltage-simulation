/* A pass through shader for the specified
 * shapes. The Position is scaled and shifted from
 * the given coordinate frame to GL normalized device
 * coordinates [-1,1]^2.
 */
#version 440 core

layout(location=1) in vec2 vP;
layout(location=2) in float vPotential;

out float potential;
const vec2 center = vec2(15, 0);
const float scale = 25;

void main() {
    gl_Position = vec4((vP-center)/scale, 0, 1);
    potential = vPotential;
}


