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

struct Vertex {
public:
    glm::vec2 Position;
    glm::vec2 TextureCoord;
    glm::vec4 Color;
};

class Player : public Drawable {
public:
    void Render(Graphics& g) {
        
    }

    bool OnMouseMove(double x, double y) {
        //printf("X: %f Y: %f\n", x, y);
        //std::cout << "X: " << x << " Y: " << y << std::endl;
        return true;
    }

    void Update(double delta) {
        
    }
};

class BouncingCube {
    glm::vec2 pos;
    glm::vec2 vel;

public:
    BouncingCube()
    {
        pos.x = rand() % 1280;
        pos.y = rand() % 720;

        vel.x = rand() % 500;
        vel.y = rand() % 500;
    }

    void Render(PrimitiveBatch<Vertex>& batch) {
        auto quad = batch.WriteQuad();

        quad[0].Position.x = pos.x;
        quad[0].Position.y = pos.y;

        quad[1].Position.x = pos.x + 2;
        quad[1].Position.y = pos.y;

        quad[2].Position.x = pos.x + 2;
        quad[2].Position.y = pos.y + 2;

        quad[3].Position.x = pos.x;
        quad[3].Position.y = pos.y + 2;
    }

    bool OnMouseMove(double x, double y) {
        return false;
    }

    void Update(double delta) {
        pos += vel * (float)delta;

        Clamp(pos.x, 0, 1280);
        Clamp(pos.y, 0, 720);

        if (pos.x == 1280 || pos.x == 0)
            vel.x *= -1.0f;

        if (pos.y == 720 || pos.y == 0)
            vel.y *= -1.0f;
    }

    void Clamp(float& inValue, float min, float max) {
        if (inValue > max)
            inValue = max;
        else if (inValue < min)
            inValue = min;
    }
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

    PrimitiveBatch<Vertex> batch(4000000, 6000000);

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

    std::vector<BouncingCube*> cubes;

    for (int i = 0; i < 100000; i++)
    {
        cubes.push_back(new BouncingCube());
    }

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
        //container.Render(nullptr);

        for (int i = 0; i < cubes.size(); i++)
        {
            cubes[i]->Update(delta);
            cubes[i]->Render(batch);
        }

        /*
        for (int i = 0; i < 100000; i++)
        {
            x = 1280.0 / i;
            y = 720.0 / i;

            Vertex* quad = batch.WriteQuad();

            quad[0].Position.x = x;
            quad[0].Position.y = y;

            quad[1].Position.x = x + 2;
            quad[1].Position.y = y;

            quad[2].Position.x = x + 2;
            quad[2].Position.y = y + 2;

            quad[3].Position.x = x;
            quad[3].Position.y = y + 2;
        }
        */
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