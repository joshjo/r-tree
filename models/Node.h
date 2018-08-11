#ifndef NODE_H
#define NODE_H


#include "Lolygon.h"
#include "Rectangle.h"


template <class T>
class Node
{
	typedef Point<T> P;
    private:
        int count;
        int maxEntries;
        Rectangle<T> *rectangle;
        Node<T> **children;
        Node<T> *father;
        Polygon<T> *polygons;
        bool leaf;
        bool intermediate;

	public:
        Node(){}
        Node(int maxEntries, int identifier)
        {
            this->count = 0;
            this->father = 0;
            this->maxEntries = maxEntries;
            int inf = 1000000;
            P min(inf,inf);
            P max(-inf,-inf);
            this->rectangle = new Rectangle<T>(min, max, identifier);
            this->children = new Node<T>*[maxEntries];
            this->polygons = new Polygon<T>[maxEntries];
            this->leaf = true;
            this->intermediate = false;
        }

        vector<Node<T>* > getChildrenVector() {
            vector<Node<T>* > array;
            for (size_t i = 0; i < count; i += 1) {
                array.push_back(children[i]);
            }
            return array;
        }

        T minDist(P & p) {
            // Calculates the mindist from the rectangle to a point
            T sx, sy, tx, ty, rx, ry;

            rx = p.x;
            ry = p.y;

            sx = rectangle->min.x;
            sy = rectangle->min.y;

            tx = rectangle->max.x;
            ty = rectangle->max.y;

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

        int size() {
            return count;
        }

        Polygon<T>* get_polygons() {
            return polygons;
        }

        Rectangle<T>* get_rectangle() {
            return rectangle;
        }

    bool isInsideRectangle(Polygon<T> *polygon)
    {
        return(rectangle->min.x <= polygon->points[0].x &&
        polygon->points[0].x <= rectangle->max.x &&
        rectangle->min.x <= polygon->points[0].y &&
        polygon->points[0].y <= rectangle->max.y
        );
    }

    //float getDistace()
    //distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));
    float getArea()
    {
        return ((rectangle->max.x - rectangle->min.x)*(rectangle->max.y - rectangle->min.y));
    }

    float getSimulatedArea(Polygon<T> * polygon)
    {
        P min = rectangle->min;
        P max = rectangle->max;

        if(polygon->points[0].x < rectangle->min.x)
            rectangle->min.x = polygon->points[0].x;

        if(polygon->points[0].x > rectangle->max.x)
            rectangle->max.x = polygon->points[0].x;

        if(polygon->points[0].y < rectangle->min.y)
            rectangle->min.y = polygon->points[0].y;

        if(polygon->points[0].y > rectangle->max.y)
            rectangle->max.y = polygon->points[0].y;

        float area = getArea();

        rectangle->min = min;
        rectangle->max = max;

        return area;
    }

    float getSimulatedArea(Node<T> * node)
    {
        float minX = rectangle->min.x;
        float maxX = rectangle->max.x;
        float minY = rectangle->min.y;
        float maxY = rectangle->max.y;

        if(node->rectangle->min.x < rectangle->min.x)
            rectangle->min.x = node->rectangle->min.x;

        if(node->rectangle->max.x > rectangle->max.x)
            rectangle->max.x = node->rectangle->max.x;

        if(node->rectangle->min.y < rectangle->min.y)
            rectangle->min.y = node->rectangle->min.y;

        if(node->rectangle->max.y > rectangle->max.y)
            rectangle->max.y = node->rectangle->max.y;

        float area = getArea();

        rectangle->min.x = minX;
        rectangle->max.x = maxX;
        rectangle->min.y = minY;
        rectangle->max.y = maxY;

        return area;
    }

    friend class RTree<T>;
};

#endif // NODE_H
