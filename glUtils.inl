#include <vector>
#include <cstdlib>
#include <GL/glew.h>

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

struct PointData {
	float potential;
	float position[2];
};

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

