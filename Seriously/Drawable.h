#pragma once
#include <vector>
#include <algorithm>
#include "Graphics.h"

class Drawable {
public:
    bool IsDead = false;
    int Layer = 0;

    virtual void Update(double delta) abstract;
    virtual void Render(Graphics& g) abstract;

    virtual bool OnMouseMove(double x, double y) { return false; }
};


class DrawableContainer {
private:
    std::vector<Drawable*> drawables;
    bool requireSorting;

public:
    void Add(Drawable* drawable) {
        drawables.push_back(drawable);
        requireSorting = true;
    }

    template<typename T>
    void Clear() {
        static_assert(std::is_base_of<Drawable, T>::value, "Skal ligesom være en drawable");

        for (int i = 0; i < drawables.size(); i++)
        {
            if (dynamic_cast<T*>(drawables[i]) != nullptr) {
                drawables[i]->IsDead = true;
            }
        }
    }

    /*
    template<typename T>
    void Get() {
        static_assert(std::is_base_of<Drawable, T>::value, "Skal ligesom være en drawable");

        for (int i = 0; i < drawables.size(); i++)
        {
            if (dynamic_cast<T*>(drawables[i]) != nullptr) {
                drawables[i]->IsDead = true;
            }
        }
    }
    */

    void OnMouseMove(float x, float y) {
        for (int i = drawables.size() - 1; i >= 0; i--)
        {
            if (drawables[i]->OnMouseMove(x, y) && !drawables[i]->IsDead)
                break;
        }
    }

    void Update(double delta) {
        for (int i = 0; i < drawables.size(); i++)
        {
            if (drawables[i]->IsDead)
                continue;

            drawables[i]->Update(delta);

            if (i > 0) {
                if (drawables[i - 1]->Layer > drawables[i]->Layer)
                    requireSorting = true;
            }
        }

        if (requireSorting) {
            /*
            std::sort(drawables.begin(), drawables.end(), [](int p) {
                return true; 

            });
            */
            /*
            std::qsort(drawables[0], drawables.size(), sizeof(Drawable*),
                [](const void* x, const void* y) {
                    return ((Drawable*)x)->Layer - ((Drawable*)y)->Layer;
                });
                */
        }
    }

    void Render(Graphics& g) {
        for (int i = drawables.size() - 1; i >= 0; i--)
        {
            if (drawables[i]->IsDead)
                continue;

            drawables[i]->Render(g);
        }
    }
};
