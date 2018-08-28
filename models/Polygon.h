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

        T getArea() {
            return (max.x - min.x) * (max.y - min.y);
        }

        T getDistance(Polygon<T> & other) {
            vector<P> thisbox = this->get_box();
            vector<P> otherbox = other.get_box();

            T minDistance = std::numeric_limits<T>::max();
            for(auto pointBox1 : thisbox)
            {
                for(auto pointBox2 : otherbox)
                {
                    T distance = sqrt(pow(pointBox1.x-pointBox2.x,2) + pow(pointBox1.y-pointBox2.y,2));
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
            cout << "Id: " << id << " points: ";
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

        int distanceSide(Point<T> &P, Point<T> &A, Point<T> &B){
            Point<T> a = A - B;
            Point<T> b = a.getOrthogonal();
            Point<T> minDistance;

            double coeff = (double)(a.y*b.x - a.x*b.y);
            double beta = (double)( a.x*(P.y - A.y) - a.y*(P.x - A.x) );

            beta = beta / coeff;

            Point<T> C( ((double)P.x) + beta * ((double)b.x),
                        ((double)P.y) + beta * ((double)b.y)  );
            Point<T> PC = P - C;

            // La distancia es perpendicular
            if( A.x < C.x && C.x < B.x ) return PC.length2();
            if( B.x < C.x && C.x < A.x ) return PC.length2();

            //  La distancia no es perpendicular
            Point<T> PA = P - A;
            Point<T> PB = P - B;

            T disPA = PA.length2();
            T disPB = PB.length2();

            if( disPA<disPB ) return disPA;
            else              return disPB;
        }

        T minDist(P & p) {
            T minimum = inf;
            int pointsSize = points.size();
            if (pointsSize == 1) {
                P diff = p - points[0];
                return diff.length2();
            }
            for (int i = 0; i < pointsSize; i++) {
                T currDist = distanceSide(p, points[i], points[(i + 1) % pointsSize]);
                if (currDist < minimum) {
                    minimum = currDist;
                }
            }
            return minimum;
            // This functions is repeated in the node.
            // T sx, sy, tx, ty, rx, ry;

            // rx = p.x;
            // ry = p.y;

            // sx = min.x;
            // sy = min.y;

            // tx = max.x;
            // ty = max.y;

            // if (p.x < sx) {
            //     rx = sx;
            // } else if (p.x > tx) {
            //     rx = tx;
            // }

            // if (p.y < sy) {
            //     ry = sy;
            // } else if (p.y > ty) {
            //     ry = ty;
            // }

            // return pow(abs(p.x - rx), 2) + pow(abs(p.y - ry), 2);
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
