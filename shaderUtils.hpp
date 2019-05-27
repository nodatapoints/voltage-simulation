#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#include <GL/glew.h>

struct {
	GLuint main, shape, compute;
} programs;

struct {
	struct {
		GLuint nPixels, windowSize, tick, alpha;
	} compute;
	struct {
		GLuint nPixels, windowSize, tick;
	} shape, main;
} uniforms;

void init();

GLuint compileShader(const GLenum type, const GLchar* source, GLint length);
GLuint linkShaders(const std::vector<GLuint>& handles);
std::string loadFile(const std::string& filename);
GLuint loadShader(const std::string& filename, GLenum type);

