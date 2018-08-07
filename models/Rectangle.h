#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"

template <class T> class RTree;
template <class T> class Node;
template <class T>
class Point;
template <class T>
class Rectangle
{
	typedef Point<T> P;
    private:
        int identifier;
        P min = P();
        P max = P();
    public:
        Rectangle(){}
        Rectangle(P pMin, P pMax, int identifier)
        {
            this->min = pMin;
            this->max = pMax;
            this->identifier = identifier;
        }
    friend class RTree<T>;
    friend class Node<T>;
};

#endif // RECTANGLE_H
