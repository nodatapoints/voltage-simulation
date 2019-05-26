#include <fstream>
#include <vector>
#include <cstdlib>

#include <GL/glew.h>
#include <SFML/Window.hpp>

#include "shaderUtils.hpp"


void handleEvent(sf::Event& e, sf::Window& w) {
    if (e.type == sf::Event::Closed ||
            (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape))
        w.close();
}

template<GLenum bufferType, typename T>
void initSSBO(GLuint target, GLuint binding, GLsizei size, GLenum type, const T value) {
    glBindBuffer(bufferType, target);
    glBufferData(bufferType, size*sizeof(T), nullptr, GL_STATIC_DRAW);

    glClearBufferData(bufferType, GL_R32F, GL_RED, type, &value);
    glBindBufferBase(bufferType, binding, target);
}

int main(int , char* []) {
    // CONSTANTS
    const int local_size = 128;

    // CREATE WINDOW
    sf::Window window(
        sf::VideoMode::getDesktopMode(), "Voltage", sf::Style::Fullscreen,
        sf::ContextSettings(8, 8, 4, 4, 4, sf::ContextSettings::Debug)
    );
    window.setMouseCursorVisible(false);
    init();

    // LOAD SHADERS
    auto vert_s = loadShader("main.vs", GL_VERTEX_SHADER);
    auto frag_s = loadShader("main.fs", GL_FRAGMENT_SHADER);
    auto sp = linkShaders({vert_s, frag_s});
    auto winsize = glGetUniformLocation(sp, "winsize");
    auto tick = glGetUniformLocation(sp, "tick");
    auto pixcount = glGetUniformLocation(sp, "pixcount");

    auto shapeVert_s = loadShader("shape.vs", GL_VERTEX_SHADER);
    auto shapeFrag_s = loadShader("shape.fs", GL_FRAGMENT_SHADER);
	auto shapeProg = linkShaders({shapeVert_s, shapeFrag_s}); 
    auto shape_winsize = glGetUniformLocation(shapeProg, "winsize");
    auto shape_pixcount = glGetUniformLocation(shapeProg, "pixcount");

    auto comp_s = loadShader("compute.glsl", GL_COMPUTE_SHADER);
    auto comp = linkShaders({comp_s});
    auto comp_winsize = glGetUniformLocation(comp, "comp_winsize");
    auto comp_tick = glGetUniformLocation(comp, "comp_tick");
    auto comp_pixcount = glGetUniformLocation(comp, "comp_pixcount");
    auto comp_alpha = glGetUniformLocation(comp, "comp_alpha");

    // SET UP VERTEX BUFFER AND ARRAY
    std::vector<float> vertices = {
        -1.0, -1.0,
        1.0, -1.0,
        -1.0, 1.0,
        1.0, 1.0
    };

    struct PointData {
        float potential;
        float position[2];
    };

	std::vector<PointData> shapeVertices = {
        {1, {0, 0}},
        {1, {.6, .2}},
        {1, {-.2, .3}},
        {-1, {.3, .4}},
        {1, {.5, .45}},
        {-5, {.34, .3}},
	};

	enum {main, shapePos, shapePotential, _num_vbos};  // aber im Buch haben die es auch so gemacht, tim
	GLuint vbos[_num_vbos];
	GLuint vaos[_num_vbos];

    glGenBuffers(_num_vbos, vbos);
    glGenVertexArrays(_num_vbos, vaos);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[shapePos]);
    glBufferData(GL_ARRAY_BUFFER, shapeVertices.size()*sizeof(PointData), shapeVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vaos[shapePos]);

    glVertexAttribPointer(shapePotential, 1, GL_FLOAT, GL_FALSE, sizeof(PointData), 0);
    glVertexAttribPointer(shapePos, 2, GL_FLOAT, GL_FALSE, sizeof(PointData), (void*) sizeof(float));    
    glEnableVertexAttribArray(shapePos);
    glEnableVertexAttribArray(shapePotential);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[main]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vaos[main]);
    glEnableVertexAttribArray(main);
    glVertexAttribPointer(main, 2, GL_FLOAT, GL_FALSE, 0, 0);    

    struct {
        GLuint potential;
        GLuint initialPotential;
        GLuint isStatic;
        GLuint isStaticUniform;
    } buffers;
    glGenBuffers(sizeof(buffers)/sizeof(GLuint), (GLuint*) &buffers);

    int nPixels = window.getSize().x * window.getSize().y;

    initSSBO<GL_SHADER_STORAGE_BUFFER>(buffers.potential, 3, 2*nPixels, GL_FLOAT, 0.0f);
    initSSBO<GL_SHADER_STORAGE_BUFFER>(buffers.isStatic, 4, nPixels, GL_BOOL, 0);

    // MAIN LOOP
    int compute_count, current_tick = -1;
    sf::Event event;

    glUseProgram(shapeProg);
    glUniform2i(shape_winsize, window.getSize().x, window.getSize().y);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[shapePos]);
    glBindVertexArray(vaos[shapePos]);

    glDrawArrays(GL_TRIANGLES, 0, shapeVertices.size());

    while (window.isOpen()) {
        while (window.pollEvent(event))
            handleEvent(event, window);

        compute_count = nPixels / 128;

		glUseProgram(comp);
        glUniform2i(comp_winsize, window.getSize().x, window.getSize().y);
        glUniform1i(comp_pixcount, nPixels);
        glUniform1i(comp_tick, ++current_tick % 2);
        glUniform1f(comp_alpha, .1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(compute_count, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(sp);
        glUniform2i(winsize, window.getSize().x, window.getSize().y);
        glUniform1i(pixcount, nPixels);
        glUniform1i(tick, current_tick % 2);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[main]);
        glBindVertexArray(vaos[main]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()/2);

        window.display();
    }
}
