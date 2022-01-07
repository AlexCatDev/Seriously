#pragma once
#include "GLBuffer.h"

template<typename T>
class PrimitiveBatch {
private:
    T* vertexPool;
    uint32_t* indexPool;

    GLBuffer<T>* vertexBuffer;
    GLBuffer<uint32_t>* indexBuffer;

    uint32_t VertexRenderCount, IndexRenderCount;

public:
    PrimitiveBatch(uint32_t vertexCount, uint32_t indexCount)
    {
        vertexBuffer = new GLBuffer<T>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, vertexCount);
        indexBuffer = new GLBuffer<uint32_t>(GL_ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW, indexCount);

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
        indexBuffer->UploadData(indexPool, 0, IndexRenderCount);
        vertexBuffer->UploadData(vertexPool, 0, VertexRenderCount);

        glDrawElements(GL_TRIANGLES, IndexRenderCount, GL_UNSIGNED_INT, nullptr);

        //Fill the vertexpool with 0's not neccersary
        memset(vertexPool, 0, VertexRenderCount * sizeof(T));
        VertexRenderCount = 0;
        IndexRenderCount = 0;

    }

    ~PrimitiveBatch() {
        delete [] vertexPool;
        delete [] indexPool;
        delete vertexBuffer;
        delete indexBuffer;
    }
};