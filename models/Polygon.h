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
    T inf = std::numeric_limits<T>::max();
    private:
        vector<P> points;
        bool intermediate;
        T id;
    public:
        P min;
        P max;
        Polygon(){}
        Polygon(vector<P> pPoints, T id)
        {
            this->points = pPoints;
            this->intermediate = false;
            this->id = id;
            getMinMax();
        }
        Polygon(P p, T id){
            this->points.push_back(p);
            this->id = id;
            this->intermediate = false;
            getMinMax();
        }

        float getDistance(Polygon<T> & other) {
            vector<P> thisbox = this->get_box();
            vector<P> otherbox = other.get_box();

            float minDistance = std::numeric_limits<T>::max();
            for(auto pointBox1 : thisbox)
            {
                for(auto pointBox2 : otherbox)
                {
                    float distance = sqrt(pow(pointBox1.x-pointBox2.x,2) + pow(pointBox1.y-pointBox2.y,2));
                    if(minDistance > distance)
                    {
                        minDistance = distance;
                    }
                }
            }
            return minDistance;
        }


        void getMinMax(){
            T xmin = inf;
            T ymin = inf;
            T xmax = 0;
            T ymax = 0;

            if(this->points.size() == 1){
                this->min = this->points[0];
                this->max = this->points[0];
            }
            else{
                for(auto p : this->points){
                    if(p.x < xmin){
                        xmin = p.x;
                    }
                    if(p.y < ymin){
                        ymin = p.y;
                    }
                    if(p.x > xmax){
                        xmax = p.x;
                    }
                    if(p.y > ymax){
                        ymax = p.y;
                    }
                }
                P pmin(xmin,ymin);
                P pmax(xmax,ymax);
                this->min = pmin;
                this->max = pmax;
            }
        }
        int get_id(){
            return id;
        }
        void print() {
            cout << "points: ";
            for (size_t i = 0; i < points.size(); ++i) {
                points[i].print();
                cout << " - ";
            }
            cout << endl;
        }

        Polygon<T> copy()
        {
            return *(new Polygon(points, id));
        }
        vector<P> get_points(){
            return points;
        }

        string get_strid() {
            string str = to_string(id);
            return str;
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
        vector<P> get_box() {
            vector<P> points(4);
            points[0] = P(min.x, min.y);
            points[1] = P(max.x, min.y);
            points[2] = P(max.x, max.y);
            points[3] = P(min.x, max.y);
            return points;
        }
    friend class RTree<T>;
    friend class Node<T>;
};

#endif // POLYGON_H
