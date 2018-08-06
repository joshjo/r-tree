#ifndef RECTANGLE_H
#define RECTANGLE_H

template <class T> class RTree;
template <class T> class Node;
template <class T>
class Rectangle
{
    private:
        float minX;
        float maxX;
        float minY;
        float maxY;
        int identifier;

    public:
        Rectangle(){}
        Rectangle(float minX, float maxX, float minY, float maxY, int identifier)
        {
            this->minX = minX;
            this->maxX = maxX;
            this->minY = minY;
            this->maxY = maxY;
            this->identifier = identifier;
        }
    friend class RTree<T>;
    friend class Node<T>;
};

#endif // RECTANGLE_H
