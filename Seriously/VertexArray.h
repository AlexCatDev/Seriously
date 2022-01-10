#pragma once
#include "GLObject.h"
#include "GL/glew.h"
#include <vector>

class VertexArray : public GLObject {
    struct VertexMember
    {
        int Count;
        GLenum Type;
        bool Normalized;
        int Offset;
    };

private:
    std::vector<VertexMember> members;
    int totalSize = 0;
public:
    //what why, i kinda understand but, why is it like this
    template<typename T>
    void Add(int count) {
        static_assert(false);
    }

    template<>
    void Add<float>(int count) {
        members.push_back({ count, GL_FLOAT, false, totalSize });
        totalSize += sizeof(GLfloat) * count;
    }

private:
    void bind(int slot = 0) {
        glBindVertexArray(handle);
    }

    void initialize() {
        glGenVertexArrays(1, &handle);
        bind();
        
        for (int i = 0; i < members.size(); i++)
        {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, members[i].Count, members[i].Type, members[i].Normalized, totalSize, (const void*)members[i].Offset);
        }
    }

    void release() {
        glDeleteVertexArrays(1, &handle);
        handle = UninitializedHandle;
    }
};
