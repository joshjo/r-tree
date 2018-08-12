#ifndef RTREE_H
#define RTREE_H

#include "Node.h"

template <class T>
class RTree
{
    typedef Point<T> P;
    typedef Node<T> N;
    typedef Polygon<T> Poly;
    T inf = std::numeric_limits<T>::max();
    struct PointNodeDistComparison {
        PointNodeDistComparison(P point) {
            this->point = point;
        }
        bool operator() (Node<T> * i, Node<T> * j) {
            return i->minDist(point) < j->minDist(point);
        }
        P point;
    };

    struct PointPolygonDistComparison {
        PointPolygonDistComparison(P point) {
            this->point = point;
        }
        bool operator() (Polygon<T> * i, Polygon<T> * j) {
            return i->minDist(point) < j->minDist(point);
        }
        P point;
    };

private:
    Node<T> * root;
    int m;
    int M;
    int treeLevel;
    bool shouldPrintLog;

public:

    RTree(int M = 5, int m = 2) {
        this->M = M;
        this->m = m;
        root = 0;
    }

    int insert(Polygon<T> * polygon) {
        N ** searchNode = search(polygon);
        cout << "-->" << *searchNode << endl;
        if ( ! (*searchNode)) {
            (*searchNode) = new N (M, 1);
        }

        int index = (*searchNode)->addPolygon(polygon);
        if (index >= M) {
            cout << "---> overflow " << polygon->id << endl;
            int a = -1, b = -1;
            (*searchNode)->getFartherPolygons(a, b);

            N ** parent = &((*searchNode)->parent);
            N * left = new N(M, 2);
            N * right = new N(M, 3);
            Poly * polygonA = (*searchNode)->polygons[a];
            Poly * polygonB = (*searchNode)->polygons[b];

            for (int i = 0; i < (*searchNode)->count; i++) {
                Poly * currentPolygon = (*searchNode)->polygons[i];
                if ((polygonA->getDistance(*currentPolygon) < polygonB->getDistance(*currentPolygon)) && left->count <= m) {
                    left->addPolygon(currentPolygon);
                } else {
                    right->addPolygon(currentPolygon);
                }
            }

            if ( ! (*parent)) {
                cout << "new parent" << endl;
                (*parent) = new N(M, 1, false);
                (*parent)->addChildren(left);
                (*parent)->addChildren(right);
                (*searchNode) = (*parent);

                right->updateRectangle();
                right->parent = (*parent);
                cout << "root"  << this->root->children[0] << endl;
            } else {
                cout << "old parent" << (*searchNode)->rectangle->id << endl;
                (*searchNode) = left;
                reInsertNode(right);
                // for (let i )
            }
            left->updateRectangle();
            left->parent = (*parent);

            (*parent)->updateRectangle();
            cout << "right :::::" << endl;
            cout << right->rectangle->min.to_string() << endl;
            cout << right->rectangle->max.to_string() << endl;
            cout << "right :::::" << endl;
        }

        return 1;
    }

    void reInsertNode(N * node) {
        N ** current = &root;
        N * previous = *current;
        while ((*current)->rectangle->isInside(*(node->rectangle)) && ( ! (*current)->leaf)) {
            previous = *current;
            for (size_t i = 0; i < (*current)->count; i += 1) {
                current = &(*current)->children[i];
                if ((*current)->rectangle->isInside(*(node->rectangle))) {
                    break;
                }
            }
        }
        int index = previous->addChildren(node);
        node->updateRectangle();
        node->parent = previous;

        if (index >= M) {
            N * left = new N(M, 5, false);
            N * right = new N(M, 6, false);
            int a = -1, b = -1;
            // previous->getFartherChildren(a, b);
            cout << "overflow nodes" << endl;

        } else {
            cout << "nothing to do" << endl;
        }
    }

    N ** search(Polygon<T> * polygon) {
        if (! root) {
            return &root;
        }
        N ** current = &root;
        while (current) {
            N * node = (*current);
            if (node->leaf) {
                break;
            }
            float minDistance = std::numeric_limits<T>::max();
            int index = -1;
            for (int i = 0; i < node->count; i += 1) {
                float newDistance = node->children[i]->rectangle->getDistance(*polygon);
                if (newDistance < minDistance) {
                    index = i;
                    minDistance = newDistance;
                }
            }
            current = &(node->children[index]);
        }
        return current;
    }

    float getDistance( Polygon<T> * p1, Polygon<T> *p2 )
    {
        vector<P> box1 = p1->get_box();
        vector<P> box2 = p2->get_box();
        float minDistance = std::numeric_limits<T>::max();
        for(auto pointBox1 : box1)
        {
            for(auto pointBox2 : box2)
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

    void print(Node<T> *node) {
        cout<<"Identifier rectangle: R"<<node->rectangle->id;
        if (node->parent != NULL) {
            cout <<" Father: R"<< node->parent->rectangle->id;
        }
        cout<<" x:"<<node->rectangle->min.x<<"-"<<node->rectangle->max.x;
        cout<<" y:"<<node->rectangle->min.y<<"-"<<node->rectangle->max.y<<endl;

        if (node->leaf) {
            for (int i=0; i<node->count; i++) {
                cout<< "Poly: "<< node->polygons[i]->id << " <=> " << node->polygons[i]->points[0].to_string() <<endl;
            }
        } else {
            for (int i=0; i < node->count; i++) {
                print(node->children[i]);
            }
        }
    }

    void print() {
        print(root);
    }
};

#endif // RTREE_H
