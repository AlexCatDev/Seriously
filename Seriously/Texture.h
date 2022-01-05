#pragma once
#include "GLObject.h"
#include "GL/glew.h"

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
