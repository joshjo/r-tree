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
            return identifier;
        }
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
