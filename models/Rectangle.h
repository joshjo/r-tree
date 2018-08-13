#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"

template <class T> class RTree;
template <class T> class Node;
template <class T> class Point;
template <class T>
class Rectangle
{
	typedef Point<T> P;
    private:
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

        string get_strid() {
            string str = "R" + to_string(id);
            return str;
        }
        Rectangle(){}
        Rectangle(P pMin, P pMax, int id)
        {
            this->min = pMin;
            this->max = pMax;
            this->id = id;
        }

        bool isInside(Rectangle<T> & other) {
            return (
                (min.x <= other.min.x) &&
                (min.y <= other.min.y) &&
                (max.x >= other.max.x) &&
                (max.y >= other.max.y)
            );
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
                    if(distance < minDistance)
                    {
                        minDistance = distance;
                    }
                }
            }
            return minDistance;
        }

        float getDistance(Rectangle<T> & other) {
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


    friend class RTree<T>;
    friend class Node<T>;
};

#endif // RECTANGLE_H
