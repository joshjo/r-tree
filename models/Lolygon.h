#ifndef POLYGON_H
#define POLYGON_H

#include "includes.h"

template <class T>
class RTree;
template <class T>
class Node;
template <class T>
class Polygon
{
    private:
        float x;
        float y;
        bool intermediate;
        T identifier;

    public:
        Polygon(){}
        Polygon(float x, float y, T identifier)
        {
            this->x = x;
            this->y = y;
            this->intermediate = false;
            this->identifier = identifier;
        }

        Polygon<T> copy()
        {
            return Polygon(x, y, identifier);
        }
    friend class RTree<T>;
    friend class Node<T>;
};

#endif // POLYGON_H
