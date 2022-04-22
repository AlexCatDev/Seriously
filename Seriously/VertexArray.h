#pragma once
#include <vector>
#include "GLObject.h"
#include "GL/glew.h"

struct VertexMember
{
    GLenum Type;
    int Count;
    bool Normalized;
};

//Type constraint? wtf
//How to use?
template<typename T>
class VertexArray : public GLObject {
    int totalSize = 0;
    std::vector<VertexMember> layout;
    GLBuffer<T>* vertexBuffer;

    //Optimization hashmap instead of this when i know how to use them lol
    int getTypeSize(GLenum type) {
        switch (type) {
            case GL_FLOAT:
                return sizeof(float);
            break;

            default:
                throw std::exception("Type not implemented :(");
                break;
        }
    }

    void bind(int slot = 0) {
        glBindVertexArray(handle);
    }

    void initialize() {
        glGenVertexArrays(1, &handle);
        bind();
        
        vertexBuffer->Bind();

        int offset = 0;
        int stride = sizeof(T);
        for (size_t i = 0; i < layout.size(); i++)
        {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, layout[i].Count, layout[i].Type, layout[i].Normalized, stride, (const void*)offset);

            offset += getTypeSize(layout[i].Type) * layout[i].Count;
        }
    }

    void release() {
        glDeleteVertexArrays(1, &handle);
        handle = UninitializedHandle;
    }

public:
    VertexArray(std::vector<VertexMember> layout, GLBuffer<T>* vertexBuffer) : layout(layout), vertexBuffer(vertexBuffer) { }
    VertexArray(const VertexArray& vao) = delete;
};