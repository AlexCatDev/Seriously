#pragma once
#include <vector>
#include <algorithm>
#include "Drawable.h"

class DrawableContainer {
private:
    std::vector<std::shared_ptr<Drawable>> drawables;
    bool requireSorting;

public:
    DrawableContainer(const DrawableContainer& container) = delete;

    DrawableContainer()
    {

    }

    const std::vector<std::shared_ptr<Drawable>>& GetDrawables() {
        return drawables;
    }

    void Add(std::shared_ptr<Drawable> drawable) {
        drawables.push_back(drawable);
        drawable->IsDead = false;
        drawable->OnAdd(*this);
        requireSorting = true;
    }

    template<typename T>
    void Get(bool (*onGet)(T& const)) {
        //std::vector<std::shared_ptr<Drawable>> drawables;
        
        //Constraint Template to make sure T is derived from Drawable?
        static_assert(std::is_base_of<Drawable, T>::value, "T needs to derive from Drawable");

        for (int i = 0; i < drawables.size(); i++)
        {
            T* val = (T*)drawables[i].get();
            //Check if current type can be casted to T type
            //T* val = dynamic_cast<T*>(drawables[i].get());
            //If it can, invoke function
            if (val != nullptr) {
                if (onGet(*val))
                    break;
            }
        }
    }


    template<typename T>
    void Clear() {
        static_assert(std::is_base_of<Drawable, T>::value, "T needs to derive from Drawable");

        for (int i = 0; i < drawables.size(); i++)
        {
            if (dynamic_cast<T*>(drawables[i]) != nullptr) {
                drawables[i]->IsDead = true;
            }
        }
    }

    void OnMouseMove(float x, float y) {
        for (int i = drawables.size() - 1; i >= 0; i--)
        {
            if (drawables[i]->OnMouseMove(x, y) && !drawables[i]->IsDead)
                break;
        }
    }

    void Update(double delta) {
        int size = drawables.size() - 1;

        for (int i = size; i >= 0; i--)
        {
            auto drawable = drawables[i];

            if (drawable->IsDead)
            {
                drawables.erase(drawables.begin() + i);
                drawable->OnRemove();
                continue;
            }

            drawable->Update(delta);

            if (i < size) {
                if (drawable->Layer > drawables[i + 1]->Layer)
                    requireSorting = true;
            }
        }

        if (requireSorting) {
            std::sort(drawables.begin(), drawables.end(), [](std::shared_ptr<Drawable> a, std::shared_ptr<Drawable> b) {
                return a->Layer > b->Layer;
            });

            std::string log;

            log.append("Sorted: ");
            log.append(std::to_string(drawables.size()));
            log.append(" Drawables!");

            Log(log.c_str(), LogLevel::Info);

            requireSorting = false;
        }
    }

    void Render(Graphics& g) {
        for (int i = drawables.size() - 1; i >= 0; i--)
        {
            auto drawable = drawables[i];

            if (drawable->IsDead)
                continue;

            drawable->Render(g);
        }
    }
};
