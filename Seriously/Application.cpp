#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include "PrimitiveBatch.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Log.h"
#include "DrawableContainer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"
#include "MathUtils.h"
#include "BMFont.h"

class CursorTrail : public Drawable {
    struct TrailPiece {
        glm::vec2 Position;
        glm::vec4 Color;

        float Width;
        bool RemoveMe = false;

        TrailPiece(glm::vec2 pos, glm::vec4 color, float width) : Position(pos), Color(color), Width(width){}

        void Update(double delta) {
            Color.a -= 3.0f * delta;
            Width -= 32.0f * delta;
            
            MathUtils::ClampMin(Width, 0);
            MathUtils::ClampMin(Color.a, 0);

            if (Color.a == 0)
                RemoveMe = true;
        }
    };


    glm::vec2 mousePos{ 0, 0 };

    std::vector<TrailPiece> pieces;

    void Render(Graphics& g) {
        if (pieces.size() > 2)
        {
            auto verts = g.Batcher.WriteTriangleStrip((pieces.size() * 2) - 2);

                int slot = g.GetTextureSlot(nullptr);
                for (int i = 1; i < pieces.size(); i++)
                {
                    glm::vec2 difference = pieces[i].Position - pieces[i - 1].Position;
                    glm::vec2 perpen{ difference.y, -difference.x };

                    perpen = glm::normalize(perpen);

                    verts->Position = pieces[i - 1].Position - perpen * pieces[i - 1].Width;
                    verts->Color = pieces[i - 1].Color;
                    verts->Rotation = 0;
                    verts->TextureSlot = slot;
                    verts->TexCoord = { 0,0 };
                    verts->RotationOrigin = { 0,0 };
                    ++verts;

                    verts->Position = pieces[i].Position + perpen * pieces[i].Width;
                    verts->Color = pieces[i].Color;
                    verts->Rotation = 0;
                    verts->TextureSlot = slot;
                    verts->TexCoord = { 1,1 };
                    verts->RotationOrigin = { 0,0 };
                    ++verts;
                }
        }
    }

    bool OnMouseMove(double x, double y) {
        glm::vec2 mouse{ x, y };

        if (glm::length(mouse - mousePos) >= 5) {
            mousePos = mouse;
            pieces.emplace_back(mousePos, (glm::vec4)1, 8.0f);
        }

        return false;
    }

    void Update(double delta) {
        for (int i = pieces.size() - 1; i >= 0; i--)
        {
            pieces[i].Update(delta);

            if (pieces[i].RemoveMe)
                pieces.erase(pieces.begin() + i);
        }
    }
};

class Player : public Drawable {
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec4 col;

    double time;
public:
    Player() : size(64, 64), col(1.0), pos(100, 100), time(0) { }

    void Render(Graphics& g) {
        //g.DrawEllipse(pos, 0, 360, 100, 0, col);

        col.x = std::sin(time);
        col.y = std::sin(time + 2);
        col.z = std::cos(time + 2);

        g.DrawRectangle(pos, size, col, nullptr, {0, 0, 1, 1});
    }

    bool OnMouseMove(double x, double y) {
        pos.x = x;
        pos.y = y;
        //printf("X: %f Y: %f\n", x, y);
        //std::cout << "X: " << x << " Y: " << y << std::endl;
        return false;
    }

    void Update(double delta) {
        //std::cout << "Time: " << time << std::endl;
        time += delta * 2;
    }

    ~Player() {
        std::cout << "DELETED PLAYER!" << std::endl;
    }
};

class BouncingCubeDrawable : public Drawable {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec4 col;
    glm::vec2 size;
public:
    BouncingCubeDrawable()
    {
        pos.x = rand() % 1280;
        pos.y = rand() % 720;

        vel.x = rand() % 500;
        vel.y = rand() % 500;

        col.a = 1.0f;
        col.r = (rand() % 255) / 255.0f;
        col.g = (rand() % 255) / 255.0f;
        col.b = (rand() % 255) / 255.0f;

        size.x = 10;
        size.y = 10;
    }

    void Render(Graphics& g) {
        //g.DrawRectangle(pos, size, col, &Texture::Circle, {0, 0, 1, 1});
        g.DrawEllipse(pos, 0, 360, size.x, 0, col, &Texture::Circle);
    }

    bool OnMouseMove(double x, double y) {
        //printf("X: %f Y: %f\n", x, y);
        //std::cout << "X: " << x << " Y: " << y << std::endl;
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

class BaseTest {
public:
    void Print() {
        Log("BaseTest.Print", LogLevel::Error);
    }
};

class Test : public BaseTest {
public:
    void Print() {
        Log("Test.Print", LogLevel::Error);
    }
};

#define DEBUG 1

int main(void)
{
    BMFont::Font font("C:\\Users\\user\\Desktop\\roboto_bold.fnt");


    Test test;
    test.Print();
    ((BaseTest)test).Print();
    DrawableContainer container;

    Texture tex("C:\\Users\\user\\Downloads\\ECntXRSXUAYZtEE.jpg");
#if DEBUG 1
    Shader shader("C:\\Users\\user\\Desktop\\test.vert", "C:\\Users\\user\\Desktop\\test.frag");
#else
    Shader shader("./test.vert", "./test.frag");
#endif

    Graphics g;
    g.Batcher.ShouldZeroBuffer = false;

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

    /*
    auto vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
    */

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    container.Add(std::make_shared<Player>());
    container.Add(std::make_shared<CursorTrail>());

    //container.Add(std::make_shared<Player>(player));
    //container.Add(std::make_shared<Drawable>(player));

    for (int i = 0; i < 2000; i++)
    {
        container.Add(std::make_shared<BouncingCubeDrawable>());
    }

    container.Get<Player>([](Player& const player) {
        player.IsDead = true;
        std::cout << "Got player! A PLAYER YEEEEEEEEEEET" << std::endl;
        return true;
    });

    double elapsedTime = 0;
    double lastTime = 0;

    int frames = 0;

    glfwSwapInterval(0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            std::cout << "FPS: " << frames << " MS: " << (1000.0 / frames) << " Drawables: " << container.GetDrawables().size() << " Hey" << std::endl;
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

        g.Projection = projection;
        container.Render(g);

        //auto p = "cock: " + g.Batch->GetPendingTriangles();
        //Log(p);
        g.Render();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}