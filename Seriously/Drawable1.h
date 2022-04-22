#pragma once
#include <vector>
#include <algorithm>
#include "Graphics.h"
using std::shared_ptr;

class Drawable {
private:
    double time;
    double previousTime = -1;
    double delta;
    bool requireSorting;

    std::vector<shared_ptr<Drawable>> children;
public:

    bool IsDead = false;
    int Layer = 0;

    virtual void OnAdd(Drawable& container) { }
    virtual void OnRemove() { }

    void Add(shared_ptr<Drawable> drawable) {
        drawable->IsDead = false;
        children.push_back(drawable);

        drawable->OnAdd(*this);
        requireSorting = true;
    }

    double GetDelta() {
        return delta;
    }

    virtual void OnUpdate(){ }
    virtual void OnRender(Graphics& g){ }

    void UpdateChildren() {
        int size = children.size() - 1;

        for (int i = size; i >= 0; i--)
        {
            auto child = children[i];
            if (child->IsDead) {
                children.erase(children.begin() + i);
                child->OnRemove();
                continue;
            }

            child->Update(time);

            if (i > 1) {
                auto prevDepth = children[i - 1]->Layer;
                auto nowDepth = child->Layer;

                if (prevDepth > nowDepth)
                    requireSorting = true;
            }
        }

        if (requireSorting) {
            std::sort(children.begin(), children.end(), [](shared_ptr<Drawable> a, shared_ptr<Drawable> b) {
                return a->Layer > b->Layer;
                });

            requireSorting = false;
        }
    }

    void RenderChildren(Graphics& g) {
        int count = children.size() - 1;

        for (int i = count; i >= 0; i--)
        {
            auto child = children[i];

            if (child->IsDead)
                continue;

            child->Render(g);
        }
    }

    void Render(Graphics& g) {
        OnRender(g);
    }

    void Update(double time) {
        this->time = time;

        if (previousTime == -1)
            previousTime = time;

        delta = time - previousTime;

        previousTime = time;

        OnUpdate();
    }
};

class Test : public Drawable {

};