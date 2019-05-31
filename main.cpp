#include <fstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>

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

struct {
    bool fullscreen = false;
    int equiPotential = 0;
    int width = 1000;
    int height = 1000;
    float alpha = .3;
    float gamma = 1.25;
    int n = 20;
    int i = 200;
} options;

int parseOptions(int argc, char *argv[]) {
    char c;
    while ((c = getopt(argc, argv, "efg:a:w:h:n:")) != -1) {
        switch (c) {
        case 'f': options.fullscreen = true; break;
        case 'e': options.equiPotential = 1; break;
        case 'a': options.alpha = atof(optarg); break;
        case 'g': options.gamma = atof(optarg); break;
        case 'w': options.width = atof(optarg); break;
        case 'h': options.height = atof(optarg); break;
        case 'n': options.n = atof(optarg); break;
        case 'i': options.i = atof(optarg); break;
        default: 
                  std::cout << "\
USAGE  ./volt [OPTIONS] [FILE]\n\n\
OPTIONS\n\
NAME         TYPE   DESCRIPTION\n\
-a <value>   float  alpha parameter. Used to relax the simulation\n\
-g <value>   float  gamma parameter. Used to sharpen the color transitions\n\
-e                  Display equipotential lines instead of the absolute potential\n\
-n <value>   int    Number of lines between 0V and the maximum absolute potential\n\
-i <value>   int    Number of iterations per frame\n\
-w <value>   int    Width of the window\n\
-h <value>   int    Height of the window\n\
-f                  Display fullscreen\n\n\
see README.md for more information.\n";
                  return -1;
        }
    }
    return 0;
}
int main(int argc, char *argv[]) {
    if (parseOptions(argc, argv) < 0)
        return -1;

    // size of device workgroups
    const int local_size = 128;

    // create window
    auto videoMode = (options.fullscreen ?
          sf::VideoMode::getDesktopMode() : sf::VideoMode(options.width, options.height));
    auto style = (options.fullscreen ? sf::Style::Fullscreen : sf::Style::None);
    sf::Window window(
        videoMode,
        "Voltage",
        style,
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

    const auto shapeVertices = parseFile(argv[optind]);
    
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
    const int nWorkgroups = nPixels / local_size + 1;

    // Initialize the SSBOs

    // Note: Front and Backbuffers
    // Is is important to update the value of each pixel __after__ computing every
    // value. Otherwise, depending on the processing order, artefacts will emerge.
    // This is solved by buffer ping-pong-ing:
    // The data is stored twice, in a front and a back buffer. In one iteration, one
    // of them is used for reading while the other is used for writing. With each
    // iteration, the roles change. This way, there is no need to move large amounts
    // of memory and everything can be done in-place.

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
    int current_tick = 0;
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event))
            handleEvent(event, window);

        // Set up uniforms for compute shader
        glUseProgram(programs.compute);
        glUniform2i(uniforms.compute.windowSize, window.getSize().x, window.getSize().y);
        glUniform1i(uniforms.compute.nPixels, nPixels);
        glUniform1f(uniforms.compute.alpha, options.alpha);

        // update front and back buffer nIterations times in an alternating manner.
        for (int i = 0; i < options.i; ++i) {
            glUniform1i(uniforms.compute.tick, (current_tick++) % 2);  // update tick
            glDispatchCompute(nWorkgroups, 1, 1);  // actually do the computation
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);  // to prevent data races
        }

        // Draw the potential on the screen
        glUseProgram(programs.main);
        glUniform2i(uniforms.main.windowSize, window.getSize().x, window.getSize().y);
        glUniform1i(uniforms.main.nPixels, nPixels);
        glUniform1i(uniforms.main.tick, current_tick % 2);
        glUniform1f(uniforms.main.bound, bound);
        glUniform1i(uniforms.main.equiPotential, options.equiPotential);
        glUniform1f(uniforms.main.gamma, options.gamma);
        glUniform1i(uniforms.main.n, options.n);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()/2);

        window.display();
    }
    return 0;  // success yay
}
