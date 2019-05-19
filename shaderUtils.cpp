#include "shaderUtils.hpp"

void init() {
    if (glewInit() != GLEW_OK)
        abort();
}

GLuint compileShader(const GLenum type, const GLchar* source, GLint length) {
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, &source, &length);
    glCompileShader(handle);

    GLint success; glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
        abort();
    return handle;
}

GLuint linkShaders(const std::vector<GLuint>& handles) {
    auto prog_handle = glCreateProgram();
    for (auto shader : handles)
        glAttachShader(prog_handle, shader);

    glLinkProgram(prog_handle);
    GLint success; glGetProgramiv(prog_handle, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
        abort();

    for (auto shader : handles) {
        glDetachShader(prog_handle, shader);
        glDeleteShader(shader);
    }
    return prog_handle;
}

std::string loadFile(const std::string& filename) {
    std::ifstream in(filename);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        return contents;
    }
    throw errno;
}

GLuint loadShader(const std::string& filename, GLenum type) {
    auto data = loadFile(filename);
    return compileShader(type, &data[0], data.size());
}

