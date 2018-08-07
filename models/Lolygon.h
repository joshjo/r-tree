#ifndef POLYGON_H
#define POLYGON_H

#include "includes.h"
#include "Point.h"

template <class T>
class RTree;
template <class T>
class Node;
template <class T>
class Point;
template <class T>
class Polygon
{
	typedef Point<T> P;
    private:
        vector<P> points;
        bool intermediate;
        T identifier;
		P min;
		P max;
    public:
        Polygon(){}
        Polygon(vector<P> pPoints, T identifier, P pMin, P pMax)
        {
            this->points = pPoints;
            this->intermediate = false;
            this->identifier = identifier;
			this->min = pMin;
			this->max = pMax;
        }
        Polygon(P p, T identifier){
            this->points.push_back(p);
            this->identifier = identifier;
            this->intermediate = false;
            this->min = p;
			this->max = p;
        }
        Polygon<T> copy()
        {
            return Polygon(points, identifier,min,max);
        }
    friend class RTree<T>;
    friend class Node<T>;
};

#endif // POLYGON_H
