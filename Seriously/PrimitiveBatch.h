#pragma once
#include "GLBuffer.h"
#include "VertexArray.h"
#include "Log.h"

template<typename T>
class PrimitiveBatch {
private:
    T* vertexPool;
    uint32_t* indexPool;

    GLBuffer<T> vertexBuffer;
    GLBuffer<uint32_t> indexBuffer;

    VertexArray<T> vertexArray;

    uint32_t VertexRenderCount = 0, IndexRenderCount = 0;

    void ensureCapacity(int vertexCount, int indexCount)
    {
        //Index pool and indexbuffer has same length, and im lazy 
        if (IndexRenderCount + indexCount > indexBuffer.GetCapacity())
        {
            Render();
            Log("IndexBuffer ran out of space, so the whole batch has been FLUSHED", LogLevel::Warning);
            return;
        }

        //same as above
        if (VertexRenderCount + vertexCount > vertexBuffer.GetCapacity())
        {
            Render();
            Log("VertexBuffer ran out of space, so the whole batch ha been FLUSHED", LogLevel::Warning);
            return;
        }
    }
public:
    int GetPendingVertices() const {
        return VertexRenderCount;
    }

    int GetPendingIndices() const {
        return IndexRenderCount;
    }

    int GetPendingTriangles() const {
        return IndexRenderCount / 3;
    }

    bool ShouldZeroBuffer = false;

    PrimitiveBatch(uint32_t vertexCount, uint32_t indexCount, std::vector<VertexMember> vertexLayout) :
        vertexBuffer(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, vertexCount), 
        indexBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW, indexCount),
        vertexArray(vertexLayout, &vertexBuffer)
    {
        vertexPool = new T[vertexCount];
        indexPool = new uint32_t[indexCount];
    }

    /// <summary>
    /// Pls dont write more than 3 vertices uwu
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    T* WriteTriangle() {
        ensureCapacity(3, 3);

        indexPool[IndexRenderCount + 0] = VertexRenderCount + 0;
        indexPool[IndexRenderCount + 1] = VertexRenderCount + 1;
        indexPool[IndexRenderCount + 2] = VertexRenderCount + 2;

        IndexRenderCount += 3;
        VertexRenderCount += 3;
        //Return the ptr to the vertexpool starting from our current writing position
        return vertexPool + VertexRenderCount - 3;
    }

    T* WriteQuad() {
        ensureCapacity(4, 6);

        indexPool[IndexRenderCount + 0] = VertexRenderCount + 0;
        indexPool[IndexRenderCount + 1] = VertexRenderCount + 1;
        indexPool[IndexRenderCount + 2] = VertexRenderCount + 2;

        indexPool[IndexRenderCount + 3] = VertexRenderCount + 0;
        indexPool[IndexRenderCount + 4] = VertexRenderCount + 2;
        indexPool[IndexRenderCount + 5] = VertexRenderCount + 3;

        IndexRenderCount += 6;
        VertexRenderCount += 4;
        return vertexPool + VertexRenderCount - 4;
    }

    T* WriteTriangleStrip(int pointCount) {
        if (pointCount < 3)
            throw std::exception("Point count can't be less than 3");

        ensureCapacity(pointCount, (pointCount - 2) * 3);

        //First triangle
        indexPool[IndexRenderCount + 0] = VertexRenderCount + 0;
        indexPool[IndexRenderCount + 1] = VertexRenderCount + 1;
        indexPool[IndexRenderCount + 2] = VertexRenderCount + 2;

        IndexRenderCount += 3;
        for (int i = 3; i < pointCount; i++)
        {
            indexPool[IndexRenderCount + 0] = VertexRenderCount - 2 + i;
            indexPool[IndexRenderCount + 1] = VertexRenderCount - 1 + i;
            indexPool[IndexRenderCount + 2] = VertexRenderCount + 0 + i;
            IndexRenderCount += 3;
        }

        VertexRenderCount += pointCount;
        return vertexPool + VertexRenderCount - pointCount;
    }

    T* WriteTriangleFan(int pointCount) {
        if (pointCount < 3)
            throw std::exception("Point count can't be less than 3");

        ensureCapacity(pointCount, (pointCount - 2) * 3);

        //First triangle
        indexPool[IndexRenderCount + 0] = VertexRenderCount + 0;
        indexPool[IndexRenderCount + 1] = VertexRenderCount + 1;
        indexPool[IndexRenderCount + 2] = VertexRenderCount + 2;

        IndexRenderCount += 3;
        for (int i = 3; i < pointCount; i++)
        {
            indexPool[IndexRenderCount + 0] = VertexRenderCount + 0;
            indexPool[IndexRenderCount + 1] = VertexRenderCount - 1 + i;
            indexPool[IndexRenderCount + 2] = VertexRenderCount + 0 + i;
            IndexRenderCount += 3;
        }

        VertexRenderCount += pointCount;
        return vertexPool + VertexRenderCount - pointCount;
    }

    void Render() {
        vertexArray.Bind();

        indexBuffer.UploadData(indexPool, 0, IndexRenderCount);
        vertexBuffer.UploadData(vertexPool, 0, VertexRenderCount);

        glDrawElements(GL_TRIANGLES, IndexRenderCount, GL_UNSIGNED_INT, nullptr);

        //Fill the vertexpool with 0's not neccersary
        if(ShouldZeroBuffer)
            memset(vertexPool, 0, VertexRenderCount * sizeof(T));

        VertexRenderCount = 0;
        IndexRenderCount = 0;

    }

    ~PrimitiveBatch() {
        delete [] vertexPool;
        delete [] indexPool;
    }
};