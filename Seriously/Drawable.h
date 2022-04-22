#pragma once
#include "Graphics.h"
#include "DrawableContainer.h"

//Forward declaration?
class DrawableContainer;

class Drawable {
public:
    bool IsDead = false;
    int Layer = 0;

    virtual void OnAdd(DrawableContainer& container) { }
    virtual void OnRemove(){ }

    virtual void Update(double delta) abstract;
    virtual void Render(Graphics& g) abstract;

    virtual bool OnMouseMove(double x, double y) { return false; }
};