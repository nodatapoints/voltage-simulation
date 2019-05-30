#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#include <GL/glew.h>

// These beautiful helper structs define the
// various handles for all my GL objects
struct {
    GLuint main, shape, compute;
} programs;

struct {
    struct {
        GLuint nPixels, windowSize, tick, alpha;
    } compute;
    struct {
        GLuint nPixels, windowSize;
    } shape;
    struct {
        GLuint nPixels, windowSize, tick, bound, equiPotential, gamma, n;
    } main;
} uniforms;

void init();

GLuint compileShader(const GLenum type, const GLchar* source, GLint length);
GLuint linkShaders(const std::vector<GLuint>& handles);
std::string loadFile(const std::string& filename);
GLuint loadShader(const std::string& filename, GLenum type);

