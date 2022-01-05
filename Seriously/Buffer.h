/*
#pragma once

template<typename T>
class Buffer : public GLObject {
    unsigned int target, usage;
    unsigned int typeSize;
    unsigned int capacity;

public:
    Buffer(unsigned int target, unsigned int usage, unsigned int capacity);

    void UploadData(const T* data, int offset, int length);

    unsigned int GetCapacity();

    void Resize(int newCapacity);

private:
    void release();

    void bind(int slot = 0);
    void initialize();
};
*/