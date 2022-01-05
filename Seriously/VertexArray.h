#pragma once
#include "GLObject.h"
#include "GL/glew.h"

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
