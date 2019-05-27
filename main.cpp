#include <fstream>
#include <vector>
#include <cstdlib>

#include <GL/glew.h>
#include <SFML/Window.hpp>

#include "glUtils.inl"


void handleEvent(sf::Event& e, sf::Window& w) {
    if (e.type == sf::Event::Closed ||
            (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape))
        w.close();
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

    // SET UP VERTEX BUFFER AND ARRAY
    std::vector<float> vertices = {
        -1.0, -1.0,
        1.0, -1.0,
        -1.0, 1.0,
        1.0, 1.0
    };

	std::vector<PointData> shapeVertices = {
        {1, {0, 0}},
        {1, {.6, .2}},
        {.5, {-.2, .3}},
        {-1, {.3, .4}},
        {1, {.5, .45}},
        {-1, {.34, .3}},
		{-.4, {.1, .4}},
		{-.5, {.1, .5}},
		{0, {.0, .4}},
		{-.5, {.1, .5}},
		{0, {.0, .4}},
		{1, {.1, .5}}
	};

	initMainProgram();
	initShapeProgram();
	initComputeProgram();

    glGenBuffers(_nVbos, vbos);
    glGenVertexArrays(_nVbos, vaos);

	initMainVertexArray(vertices);
	initShapeVertexArrays(shapeVertices);

    const int nPixels = window.getSize().x * window.getSize().y;
    const int nWorkgroups = nPixels / 128;

    BufferObject<GL_SHADER_STORAGE_BUFFER, float> potential(3, 2*nPixels, GL_FLOAT, 0.0f);
    BufferObject<GL_SHADER_STORAGE_BUFFER, int> isStatic(4, nPixels, GL_BOOL, 0);

	drawShape(shapeVertices, window);

    int current_tick = -1;
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event))
            handleEvent(event, window);

		glUseProgram(programs.compute);
        glUniform2i(uniforms.compute.windowSize,
			window.getSize().x, window.getSize().y);
        glUniform1i(uniforms.compute.nPixels, nPixels);
        glUniform1i(uniforms.compute.tick, ++current_tick % 2);
        glUniform1f(uniforms.compute.alpha, .1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(nWorkgroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(programs.main);
        glUniform2i(uniforms.main.windowSize,
			window.getSize().x, window.getSize().y);
        glUniform1i(uniforms.main.nPixels, nPixels);
        glUniform1i(uniforms.main.tick, current_tick % 2);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[mainPos]);
        glBindVertexArray(vaos[mainPos]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()/2);

        window.display();
    }
	return 0;
}
