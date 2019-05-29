#include <fstream>
#include <vector>
#include <cstdlib>

#include <GL/glew.h>
#include <SFML/Window.hpp>

#include "glUtils.inl"
#include "parser.hpp"


void handleEvent(sf::Event& e, sf::Window& w) {
    /* Closes the window if Escape is pressed */
    if (e.type == sf::Event::Closed ||
            (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape))
        w.close();
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "no file specified" << std::endl;
        return -1;
    }

    // size of device workgroups
    const int local_size = 128;

    // create window
    sf::Window window(
        sf::VideoMode(500, 500), "Voltage", sf::Style::None,
        sf::ContextSettings(8, 8, 4, 4, 4, sf::ContextSettings::Debug)
    );
    window.setMouseCursorVisible(false);
    init();

    // To render the whole screen, we need the vertices to cover the whole
    // screen space, in order to apply the fragment shader main.fs to every
    // pixel.
    const std::vector<float> vertices = {
        -1.0, -1.0,
        1.0, -1.0,
        -1.0, 1.0,
        1.0, 1.0
    };

    const auto shapeVertices = parseFile(argv[1]);
    
    // find the biggest absolute voltage
    const auto bound = getBound(shapeVertices);

    // load all the shaders, compile the programs
    // and set up all the uniform locations
    initMainProgram();
    initShapeProgram();
    initComputeProgram();

    // Generate VBOs and VAOs for main (the screen covering rectangle)
    // and shape
    glGenBuffers(_nVbos, vbos);
    glGenVertexArrays(_nVbos, vaos);

    initMainVertexArray(vertices);
    initShapeVertexArrays(shapeVertices);

    const int nPixels = window.getSize().x * window.getSize().y;
    const int nWorkgroups = nPixels / local_size;

    // Initialize the SSBOs

    // In potential, the actual data is stored in a 1D row major representation
    // Potential contains the front and back buffer end to end, each nPixels in size.
    // When rendering, judging from current_tick %2 either being 0 or 1, data from the
    // front is read to update the back buffer, or vice versa.
    BufferObject<GL_SHADER_STORAGE_BUFFER, float> potential(3, 2*nPixels, GL_FLOAT, 0.0f);

    // isStatic is a boolean array over all pixels on the screen in 1D row major
    // representation. At a given position, iff the entry is true the potential will
    // not change in neither the back nor front buffer.
    // Note:
    //     int is used instead of float because the width of GL_BOOL is 4 on my device.
    BufferObject<GL_SHADER_STORAGE_BUFFER, int> isStatic(4, nPixels, GL_BOOL, 0);

    // Draw the shape into the Buffers.
    drawShape(shapeVertices, window);

    // put vertex buffers back to main, since from this point on only the main
    // program will render.
    glBindBuffer(GL_ARRAY_BUFFER, vbos[mainPos]);
    glBindVertexArray(vaos[mainPos]);

    // Main loop
    
    // current_tick is used to determine wether to update the front or back buffer.
    int current_tick = -1;
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event))
            handleEvent(event, window);

        // Set up uniforms for compute shader
        glUseProgram(programs.compute);
        glUniform2i(uniforms.compute.windowSize,
            window.getSize().x, window.getSize().y);
        glUniform1i(uniforms.compute.nPixels, nPixels);
        glUniform1f(uniforms.compute.alpha, .1);

        // update front and back buffer nIterations times in an alternating manner.
        for (int i = 0; i < 20; ++i) {
            glUniform1i(uniforms.compute.tick, ++current_tick % 2);  // update tick
            glDispatchCompute(nWorkgroups, 1, 1);  // actually do the computation
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);  // to prevent data races
        }

        // Draw the potential on the screen
        glUseProgram(programs.main);
        glUniform2i(uniforms.main.windowSize,
            window.getSize().x, window.getSize().y);
        glUniform1i(uniforms.main.nPixels, nPixels);
        glUniform1i(uniforms.main.tick, current_tick % 2);
        glUniform1f(uniforms.main.bound, bound);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()/2);

        window.display();
    }
    return 0;
}
