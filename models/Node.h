#ifndef NODE_H
#define NODE_H

#include "Polygon.h"
#include "Rectangle.h"


template <class T>
class Node
{
	typedef Point<T> P;
    typedef Node<T> N;
    typedef Polygon<T> Poly;
    typedef Rectangle<T> Box;
    private:
        int count;
        int M;
        N * parent;
        N ** children;
        Poly ** polygons;
        Box * rectangle;
        bool leaf;
        int id;

	public:
        Node(){}
        Node(int M, int id, bool leaf = true)
        {
            this->count = 0;
            this->parent = 0;
            this->M = M;
            this->id = id;
            T inf = std::numeric_limits<T>::max();
            P min(inf,inf);
            P max(-inf,-inf);
            this->rectangle = new Box(min, max, id);
            this->children = new N * [M + 1];
            this->polygons = new Poly * [M + 1];
            this->leaf = leaf;
        }

        void updateRectangle(bool recursive = false) {
            if (leaf) {
                for (int i = 0; i < count; i += 1) {
                    if (polygons[i]->min.x < rectangle->min.x) {
                        rectangle->min.x = polygons[i]->min.x;
                    }
                    if (polygons[i]->min.y < rectangle->min.y) {
                        rectangle->min.y = polygons[i]->min.y;
                    }
                    if (polygons[i]->max.x > rectangle->max.x) {
                        rectangle->max.x = polygons[i]->max.x;
                    }
                    if (polygons[i]->max.y > rectangle->max.y) {
                        rectangle->max.y = polygons[i]->max.y;
                    }
                }
            } else {
                for (int i = 0; i < count; i += 1) {
                    if (children[i]->rectangle->min.x < rectangle->min.x) {
                        rectangle->min.x = children[i]->rectangle->min.x;
                    }
                    if (children[i]->rectangle->min.y < rectangle->min.y) {
                        rectangle->min.y = children[i]->rectangle->min.y;
                    }
                    if (children[i]->rectangle->max.x > rectangle->max.x) {
                        rectangle->max.x = children[i]->rectangle->max.x;
                    }
                    if (children[i]->rectangle->max.y > rectangle->max.y) {
                        rectangle->max.y = children[i]->rectangle->max.y;
                    }
                }
            }
            if (parent && recursive) {
                parent->updateRectangle();
            }
        }

        int addChildren(Node * child) {
            int index = this->count;
            this->children[index] = child;
            this->count += 1;
            return index;
        }

        int addPolygon(Poly * polygon) {
            // CHeck if the count works together children with polygons
            // Technically if the node is a leaf, then the node should contain
            // only leaves.
            int index = this->count;
            this->polygons[index] = polygon;
            this->count += 1;
            return index;
        }

        void getFartherPolygons(int & a, int & b) {
            float maxDistance = std::numeric_limits<T>::min();
            for (int i = 0; i < count; i += 1) {
                for (int j = i + 1; j < count; j += 1) {
                    float newDistance = polygons[i]->getDistance(*polygons[j]);
                    if (newDistance > maxDistance) {
                        maxDistance = newDistance;
                        a = i;
                        b = j;
                    }
                }
            }
        }

        void getFartherChildren(int & a, int & b) {
            float maxDistance = std::numeric_limits<T>::min();
            for (size_t i = 0; i < count; i += 1) {
                for (size_t j = i + 1; j < count; j += 1) {
                    float newDistance = children[i]->rectangle->getDistance(*(children[j]->rectangle));
                    if (newDistance > maxDistance) {
                        maxDistance = newDistance;
                        a = i;
                        b = j;
                    }
                }
            }
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

        /*Poly* get_polygons() {
            return polygons;
        }*/

        Rectangle<T>* get_rectangle() {
            return rectangle;
        }

        //for rangeSearch --Luis--
        vector<Polygon<T>*> get_vector_polygons(){
            vector<Polygon<T>*> array;
            for (size_t i = 0; i < count; i += 1) {
                array.push_back(polygons[i]);
            }
            return array;
        }
        // bool isInsideRectangle(Node<T> *node)
        // {
        //     return(rectangle->min.x <= node->re->points[0].x &&
        //     polygon->points[0].x <= rectangle->max.x &&
        //     rectangle->min.x <= polygon->points[0].y &&
        //     polygon->points[0].y <= rectangle->max.y
        //     );
        // }

    // //float getDistace()
    // //distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));
    T getArea() {
        return ((
            rectangle->max.x - rectangle->min.x
        ) * (rectangle->max.y - rectangle->min.y));
    }

    T deadArea() {
        T polygonsArea = 0;
        for (size_t i = 0; i < count; i += 1) {
            polygonsArea += polygons[i]->getArea();
        }
        return getArea() - polygonsArea;
    }

    T getSimulatedArea(Polygon<T> * polygon)
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

        T area = getArea();

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
