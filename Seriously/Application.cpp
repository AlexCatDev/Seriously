#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "PrimitiveBatch.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Log.h"
#include "Drawable.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"

class Player : public Drawable {
public:
    void Render() {
        
    }

    bool OnMouseMove(double x, double y) {
        //printf("X: %f Y: %f\n", x, y);
        //std::cout << "X: " << x << " Y: " << y << std::endl;
        return true;
    }

    void Update(double delta) {
        
    }
};

struct Vertex {
public:
    glm::vec2 Position;
    glm::vec2 TextureCoord;
    glm::vec4 Color;
};

int main(void)
{
    DrawableContainer container;

    VertexArray vao;
    //pos
    vao.Add<float>(2);
    //texcoord
    vao.Add<float>(2);
    //color
    vao.Add<float>(4);

    PrimitiveBatch<Vertex> batch(4000, 6000);

    Texture tex("penis");
    Shader shader("C:\\Users\\user\\Desktop\\test.vert", "C:\\Users\\user\\Desktop\\test.frag");

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "alien language", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    Player player;
    container.Add(&player);

    double elapsedTime = 0;
    double lastTime = 0;

    int frames = 0;

    glfwSwapInterval(0);
    //glfwSetWindowSizeCallback(window, glfwwindowsizefun)
    

    double lastX = 0, lastY = 0;
    double x = 0, y = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glm::mat4 projection = glm::ortho<float>(0, windowWidth, windowHeight, 0, -1, 1);
        
        double time = glfwGetTime();

        double delta = time - lastTime;
        lastTime = time;

        elapsedTime += delta;

        frames++;

        if (elapsedTime >= 1.0) {
            std::cout << "FPS: " << frames << std::endl;
            elapsedTime -= 1;
            frames = 0;
        }

        /* Poll for and process events */
        glfwPollEvents();

        glfwGetCursorPos(window, &x, &y);
        
        if (x != lastX || y != lastY) {
            lastX = x;
            lastY = y;

            container.OnMouseMove(x, y);
        }

        container.Update(delta);
        container.Render();

        Vertex* quad = batch.WriteQuad();

        quad[0].Position.x = x;
        quad[0].Position.y = y;
        quad[0].Color.a = 1.0f;
        quad[0].Color.r = 1.0f;

        quad[1].Position.x = x + 100;
        quad[1].Position.y = y;

        quad[2].Position.x = x + 100;
        quad[2].Position.y = y + 100;

        quad[3].Position.x = x;
        quad[3].Position.y = y + 100;

        //Log("Test", LogLevel::Info);
        //printf("%p\n", tri);

        shader.Bind();
        shader.SetMatrix("u_Projection", projection);

        batch.Render();
        vao.Bind();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}