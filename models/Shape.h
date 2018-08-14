#ifndef SHAPE_H
#define SHAPE_H

#include "Point.h"

template <class T> class RTree;
template <class T> class Node;
template <class T> class Point;
template <class T>
class Shape {
    typedef Point<T> P;
protected:
    int id;
    P min = P();
    P max = P();
public:
    P get_min() {
        return min;
    }
    P get_max() {
        return max;
    }
    vector<P> get_box() {
        vector<P> points(4);
        points[0] = P(min.x, min.y);
        points[1] = P(max.x, min.y);
        points[2] = P(max.x, max.y);
        points[3] = P(min.x, max.y);
        return points;
    }
    int get_id() {
        return id;
    }
    Shape(){}
    Shape(P & pMin, P & pMax, int id)
    {
        this->min = pMin;
        this->max = pMax;
        this->id = id;
    }

    friend class RTree<T>;
    friend class Node<T>;
};

#endif // SHAPE_H
