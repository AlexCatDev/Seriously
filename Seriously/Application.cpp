#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "GLBuffer.h"

class GLObject {
protected:
    unsigned int handle;

    virtual void initialize() abstract;
    virtual void bind(int slot = 0) abstract;
    virtual void release() abstract;

public:
    /// <summary>
    /// Bind an object
    /// </summary>
    /// <param name="slot">The slot to bind the object to, only used for textures</param>
    void Bind(int slot = 0) {
        if (IsInitialized())
            bind();
        else
            initialize();
    }

    bool IsInitialized() {
        return handle != 0;
    }

    void Release() {
        if (IsInitialized())
            release();

        //_ASSERT(IsInitialized);
    }

    ~GLObject() {
        Release();
    }
};

template<typename T>
class Buffer : public GLObject {
    unsigned int target, usage;
    unsigned int typeSize;
    unsigned int capacity;

public:
    Buffer(unsigned int target, unsigned int usage, unsigned int capacity)
    {
        this->target = target;
        this->usage = usage;
        this->capacity = capacity;

        typeSize = sizeof(T);
    }

    void UploadData(const T* data, int offset, int length) {
        Bind();
        glBufferSubData(target, offset, length * typeSize, data);
    }

    unsigned int GetCapacity() { return capacity; }

    void Resize(int newCapacity) {
        if (IsInitialized()) {
            bind();
            glBufferData(target, newCapacity * typeSize, nullptr, usage);
        }

        this->capacity = newCapacity;
    }

private:
    void release() {
        glDeleteBuffers(1, &handle);
        handle = 0;
    }

    void bind(int slot = 0) {
        glBindBuffer(target, handle);
    }

    void initialize() {
        glGenBuffers(1, &handle);
        bind();

        glBufferData(target, capacity * typeSize, nullptr, usage);
    }
};

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
class PrimitiveBatch {
private:
    T* vertexPool;
    uint32_t* indexPool;

    Buffer<T> *vertexBuffer;
    Buffer<uint32_t> *indexBuffer;

    uint32_t VertexRenderCount, IndexRenderCount;

public:
    PrimitiveBatch(uint32_t vertexCount, uint32_t indexCount)
    {
        vertexBuffer = new Buffer<T>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, vertexCount);
        indexBuffer = new Buffer<uint32_t>(GL_ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW, indexCount);

        vertexPool = new T[vertexCount];
        indexPool = new uint32_t[indexCount];

        VertexRenderCount = 0;
        IndexRenderCount = 0;
    }

    /// <summary>
    /// Pls dont write more than 3 vertices uwu
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    T* WriteTriangle() {
        indexPool[IndexRenderCount++] = VertexRenderCount + 0;
        indexPool[IndexRenderCount++] = VertexRenderCount + 1;
        indexPool[IndexRenderCount++] = VertexRenderCount + 2;

        VertexRenderCount += 3;
        //Return the ptr to the vertexpool starting from our current writing position
        return vertexPool + VertexRenderCount - 3;
    }

    T* WriteQuad() {
        indexPool[IndexRenderCount++] = VertexRenderCount + 0;
        indexPool[IndexRenderCount++] = VertexRenderCount + 1;
        indexPool[IndexRenderCount++] = VertexRenderCount + 2;

        indexPool[IndexRenderCount++] = VertexRenderCount + 0;
        indexPool[IndexRenderCount++] = VertexRenderCount + 2;
        indexPool[IndexRenderCount++] = VertexRenderCount + 3;

        VertexRenderCount += 4;
        return vertexPool + VertexRenderCount - 4;
    }

    T* WriteTriangleStrip(int pointCount) {
        if (pointCount < 3)
            throw std::exception("Point count can't be less than 3");

        //First triangle
        indexPool[IndexRenderCount++] = VertexRenderCount + 0;
        indexPool[IndexRenderCount++] = VertexRenderCount + 1;
        indexPool[IndexRenderCount++] = VertexRenderCount + 2;

        for (int i = 3; i < pointCount; i++)
        {
            indexPool[IndexRenderCount++] = VertexRenderCount - 2 + i;
            indexPool[IndexRenderCount++] = VertexRenderCount - 1 + i;
            indexPool[IndexRenderCount++] = VertexRenderCount + 0 + i;
        }

        VertexRenderCount += pointCount;
        return vertexPool + VertexRenderCount - pointCount;
    }

    T* WriteTriangleFan(int pointCount) {
        if (pointCount < 3)
            throw std::exception("Point count can't be less than 3");

        //First triangle
        indexPool[IndexRenderCount++] = VertexRenderCount + 0;
        indexPool[IndexRenderCount++] = VertexRenderCount + 1;
        indexPool[IndexRenderCount++] = VertexRenderCount + 2;

        for (int i = 3; i < pointCount; i++)
        {
            indexPool[IndexRenderCount++] = VertexRenderCount + 0;
            indexPool[IndexRenderCount++] = VertexRenderCount - 1 + i;
            indexPool[IndexRenderCount++] = VertexRenderCount + 0 + i;
        }

        VertexRenderCount += pointCount;
        return vertexPool + VertexRenderCount - pointCount;
    }

    void Render() {
        vertexBuffer->UploadData(vertexPool, 0, VertexRenderCount);
        indexBuffer->UploadData(indexPool, 0, IndexRenderCount);

        glDrawElements(GL_TRIANGLES, IndexRenderCount, GL_UNSIGNED_INT, nullptr);

        //Fill the vertexpool with 0's not neccersary
        memset(vertexPool, 0, VertexRenderCount * sizeof(T));
        VertexRenderCount = 0;
        IndexRenderCount = 0;

    }

    ~PrimitiveBatch() {
        delete vertexPool;
        delete indexPool;
        delete vertexBuffer;
        delete indexBuffer;
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
    float X, Y;
    float TexX, TexY;
    float R, G, B, A;
};

char* splitString(const char* input, int inputLen, int startIndex) {
    int newLen = inputLen - startIndex;
    char* newString = new char[newLen + 1];

    memcpy(newString, &input[startIndex], newLen);

    newString[newLen] = '\0';

    return newString;
}

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

        auto tri = batch.WriteTriangle();
        tri->X = -0.5f;
        tri->Y = -0.5f;
        ++tri;

        tri->X = 0.5f;
        tri->Y = -0.5f;
        ++tri;

        tri->X = 0.0f;
        tri->Y = 0.5f;

        batch.Render();

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}