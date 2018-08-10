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
        int get_id(){
            return identifier;
        }
        void print() {
            cout << "points: ";
            for (size_t i = 0; i < points.size(); ++i) {
                points[i].print();
                cout << " - ";
            }
            // cout << endl << "min: ";
            // min.print();
            // cout << endl << "max: ";
            // max.print();
            cout << endl;
        }

        Polygon<T> copy()
        {
            return *(new Polygon(points, identifier, min, max));
        }
        vector<P> get_points(){
            return points;
        }

        T minDist(P & p) {
            // This functions is repeated in the node.
            T sx, sy, tx, ty, rx, ry;

            rx = p.x;
            ry = p.y;

            sx = min.x;
            sy = min.y;

            tx = max.x;
            ty = max.y;

            if (p.x < sx) {
                rx = sx;
            } else if (p.x > tx) {
                rx = tx;
            }

            if (p.y < sy) {
                ry = sy;
            } else if (p.y > ty) {
                ry = ty;
            }

            return pow(abs(p.x - rx), 2) + pow(abs(p.y - ry), 2);
        }

    friend class RTree<T>;
    friend class Node<T>;
};

#endif // POLYGON_H
