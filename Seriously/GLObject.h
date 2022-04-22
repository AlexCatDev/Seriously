#pragma once

class GLObject
{
#define UninitializedHandle -6969

protected:
    unsigned int handle = UninitializedHandle;

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
        return handle != UninitializedHandle;
    }

    void Release() {
        if (IsInitialized())
            release();
    }

    ~GLObject() {
        Release();
    }
};

