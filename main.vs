#version 440 core

layout(location = 0) in vec2 vP;

void main() {
    gl_Position = vec4(vP.xy, 0, 1);
}

