#ifndef NODE_H
#define NODE_H


#include "Lolygon.h"
#include "Rectangle.h"


template <class T>
class Node
{
    private:
        int count;
        int maxEntries;
        Rectangle<T> *rectangle;
        Node *children;
        Node *father;
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
            this->rectangle = new Rectangle<T>(INF, -INF, INF, -INF, identifier);
            this->children = new Node<T>[maxEntries];
            this->polygons = new Polygon<T>[maxEntries];
            this->leaf = true;
            this->intermediate = false;
        }

    bool isInsideRectangle(Polygon<T> *polygon)
    {
        return(rectangle->minX <= polygon->x &&
        polygon->x <= rectangle->maxX &&
        rectangle->minY <= polygon->y &&
        polygon->y <= rectangle->maxY
        );
    }

    //float getDistace()
    //distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));
    float getArea()
    {
        return ((rectangle->maxX - rectangle->minX)*(rectangle->maxY - rectangle->minY));
    }

    float getSimulatedArea(Polygon<T> * polygon)
    {
        float minX = rectangle->minX;
        float maxX = rectangle->maxX;
        float minY = rectangle->minY;
        float maxY = rectangle->maxY;

        if(polygon->x < rectangle->minX)
            rectangle->minX = polygon->x;

        if(polygon->x > rectangle->maxX)
            rectangle->maxX = polygon->x;

        if(polygon->y < rectangle->minY)
            rectangle->minY = polygon->y;

        if(polygon->y > rectangle->maxY)
            rectangle->maxY = polygon->y;

        float area = getArea();

        rectangle->minX = minX;
        rectangle->maxX = maxX;
        rectangle->minY = minY;
        rectangle->maxY = maxY;

        return area;
    }

    friend class RTree<T>;
};

#endif // NODE_H
