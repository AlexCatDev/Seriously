#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "PrimitiveBatch.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Log.h"

class Texture : public GLObject {
    const char* filename;
    unsigned int width, height;
    unsigned int internalFormat, format, pixelType;

public:
    Texture(int width, int height,
        unsigned int internalFormat = GL_RGBA, unsigned int format = GL_RGBA, unsigned int pixelType = GL_UNSIGNED_BYTE)
    {
        this->width = width;
        this->height = height;
        this->internalFormat = internalFormat;
        this->format = format;
        this->pixelType = pixelType;

        this->filename = nullptr;
    }
    
    Texture(const char* filename) {
        this->filename = filename;
    }
    
    int GetWidth() { return width; }
    int GetHeight() { return height; }

private:
    void release() {
        glDeleteTextures(1, &handle);
        handle = 0;
    }

    void bind(int slot = 0) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, handle);
    }

    void initialize() {
        glGenTextures(1, &handle);
        bind();

        if (filename == nullptr)
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelType, nullptr);
        else {
            //Load image from file
            width = 69;
            height = 1337;
            void* data = nullptr;

            //Upload
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

    }
};

class Drawable {
public:
    virtual void Update(double delta) abstract;
    virtual void Render() abstract;

    virtual bool OnMouseMove(double x, double y) { return false; }
};

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

template<typename T>
class VertexArray : public GLObject {
public:
    VertexArray()
    {

    }

    void SetLayout() {

    }
private:
    void bind(int slot) {
        glBindVertexArray(handle);
    }

    void initialize() {
        glGenVertexArrays(1, &handle);
    }

    void release() {
        glDeleteVertexArrays(1, &handle);
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
    PrimitiveBatch<Vertex> batch(4000, 6000);
    Vertex* vertices1 = batch.WriteTriangle();
    Vertex* vertices2 = batch.WriteTriangle();
    Vertex* vertices3 = batch.WriteTriangleFan(3);
    
    printf("%p\n", vertices1);
    printf("%p\n", vertices2);

    Texture tex("penis");

    std::vector<Drawable*> drawables;

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
    drawables.push_back(&player);

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

        glm::mat4 projection = glm::ortho(0, windowWidth, windowHeight, 0, -1, 1);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
        
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

            for (int i = drawables.size() - 1; i >= 0; i--)
            {
                if (drawables[i]->OnMouseMove(x, y))
                    break;
            }
        }

        for (int i = 0; i < drawables.size(); i++)
        {
            drawables[i]->Update(delta);
            drawables[i]->Render();
        }

        Vertex* tri = batch.WriteTriangle();
        Log("Test", LogLevel::Info);
        //printf("%p\n", tri);

        batch.Render();

        glBegin(GL_TRIANGLES);
        /*
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0, 0.5f);
        */

        glVertex2f(x, y);
        glVertex2f(50 + x, y);
        glVertex2f(25 + x, 50 + y);

        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}