#pragma once
class GLHandle
{
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

    ~GLHandle() {
        Release();
    }
};

