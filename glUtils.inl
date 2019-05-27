#include <vector>
#include <cstdlib>
#include <SFML/Window.hpp>
#include <GL/glew.h>

#include "shaderUtils.hpp"
#include "parser.hpp"

template<GLenum bufferType, typename T>
class BufferObject {
public:
    GLuint handle;
    BufferObject(GLuint binding, GLsizei size, GLenum type, const T value) {
		handle = binding;

		glGenBuffers(1, &handle);
		glBindBuffer(bufferType, handle);

		glBufferData(bufferType, size*sizeof(T), nullptr, GL_STATIC_DRAW);
		glClearBufferData(bufferType, GL_R32F, GL_RED, type, &value);

		glBindBufferBase(bufferType, binding, handle);
	}
    ~BufferObject() { glDeleteBuffers(1, &handle); };
	operator GLuint() const { return handle; }
};

enum {mainPos, shapePos, shapePotential, _nVbos};
GLuint vbos[_nVbos];
GLuint vaos[_nVbos];

void initShapeVertexArrays(const std::vector<PointData> &shapeVertices) {
    glBindBuffer(GL_ARRAY_BUFFER, vbos[shapePos]);
    glBufferData(GL_ARRAY_BUFFER, shapeVertices.size()*sizeof(PointData), shapeVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vaos[shapePos]);

    glVertexAttribPointer(shapePotential, 1, GL_FLOAT, GL_FALSE, sizeof(PointData), 0);
    glVertexAttribPointer(shapePos, 2, GL_FLOAT, GL_FALSE, sizeof(PointData), (void*) sizeof(float));
    glEnableVertexAttribArray(shapePos);
    glEnableVertexAttribArray(shapePotential);
}

void initMainVertexArray(const std::vector<float> &vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, vbos[mainPos]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vaos[mainPos]);

    glVertexAttribPointer(mainPos, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mainPos);
}

void initMainProgram() {
	auto vert_s = loadShader("main.vs", GL_VERTEX_SHADER);
	auto frag_s = loadShader("main.fs", GL_FRAGMENT_SHADER);
	programs.main = linkShaders({vert_s, frag_s});
	uniforms.main.windowSize = glGetUniformLocation(programs.main, "windowSize");
	uniforms.main.tick = glGetUniformLocation(programs.main, "tick");
	uniforms.main.nPixels = glGetUniformLocation(programs.main, "nPixels");
}

void initShapeProgram() {
	auto shapeVert_s = loadShader("shape.vs", GL_VERTEX_SHADER);
	auto shapeFrag_s = loadShader("shape.fs", GL_FRAGMENT_SHADER);
	programs.shape = linkShaders({shapeVert_s, shapeFrag_s});
	uniforms.shape.windowSize = glGetUniformLocation(programs.shape, "windowSize");
	uniforms.shape.nPixels = glGetUniformLocation(programs.shape, "nPixels");
}

void initComputeProgram() {
	auto comp_s = loadShader("compute.glsl", GL_COMPUTE_SHADER);
	programs.compute = linkShaders({comp_s});
	uniforms.compute.windowSize = glGetUniformLocation(programs.compute, "windowSize");
	uniforms.compute.tick = glGetUniformLocation(programs.compute, "tick");
	uniforms.compute.nPixels = glGetUniformLocation(programs.compute, "nPixels");
	uniforms.compute.alpha = glGetUniformLocation(programs.compute, "alpha");
}

void drawShape(const std::vector<PointData> &shapeVertices, const sf::Window &window) {
    glUseProgram(programs.shape);
    glUniform2i(uniforms.shape.windowSize, window.getSize().x, window.getSize().y);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[shapePos]);
    glBindVertexArray(vaos[shapePos]);

    glDrawArrays(GL_TRIANGLES, 0, shapeVertices.size());
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
