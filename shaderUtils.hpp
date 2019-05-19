#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#include <GL/glew.h>


void init();

GLuint compileShader(const GLenum type, const GLchar* source, GLint length);
GLuint linkShaders(const std::vector<GLuint>& handles);
std::string loadFile(const std::string& filename);
GLuint loadShader(const std::string& filename, GLenum type);

