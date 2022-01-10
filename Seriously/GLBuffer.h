#pragma once
#include "GLObject.h"
#include <GL/glew.h>

template<typename T>
class GLBuffer :
    public GLObject
{
    unsigned int target, usage;
    unsigned int typeSize;
    unsigned int capacity;

public:
    GLBuffer(unsigned int target, unsigned int usage, unsigned int capacity)
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
        handle = UninitializedHandle;
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

