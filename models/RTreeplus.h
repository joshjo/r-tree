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
    int id;

public:

    RTree(int M = 5, int m = 2) {
        this->M = M;
        this->m = m;
        root = 0;
        id = 1;
        shouldPrintLog = false;
    }

    int insert(Polygon<T> * polygon) {
        if (polygon->id == 9) {
            shouldPrintLog = true;
        }
        int idRegion=0;
        N ** searchNode = search(polygon);
        if ( ! (*searchNode)) {
            (*searchNode) = new N (M, id++);
            idRegion = (*searchNode)->get_rectangle()->get_id();
        }
        
        int index = (*searchNode)->addPolygon(polygon);
        if (index >= M) {
            int a = -1, b = -1;
            (*searchNode)->getFartherPolygons(a, b);

            N ** parent = &((*searchNode)->parent);
            N * left = new N(M, id++);
            N * right = new N(M, id++);
            Poly * polygonA = (*searchNode)->polygons[a];
            Poly * polygonB = (*searchNode)->polygons[b];

            cout << "polygonA " << polygonA->id << endl;
            cout << "polygonB " << polygonB->id << endl;

            // if (shouldPrintLog) {
            //     cout << "polygonA " << polygonA->id << endl;
            //     cout << "polygonB " << polygonB->id << endl;
            // }

            left->addPolygon(polygonA);
            right->addPolygon(polygonB);

            for (int i = 0; i < (*searchNode)->count; i++) {
                
                //cout << "idright " << right->get_rectangle()->get_id() << endl;
                if (i == a || i == b) {
                    continue;
                }
                Poly * currentPolygon = (*searchNode)->polygons[i];
                if ((polygonA->getDistance(*currentPolygon) < polygonB->getDistance(*currentPolygon)) && left->count <= m) {
                    left->addPolygon(currentPolygon);
                } else if (right->count <= m) {
                    right->addPolygon(currentPolygon);
                } else {
                    left->addPolygon(currentPolygon);
                }

            }

            if ( ! (*parent)) {
                (*parent) = new N(M, id++, false);
                (*parent)->addChildren(left);
                (*parent)->addChildren(right);
                (*searchNode) = (*parent);
                left->updateRectangle();
                left->parent = (*parent);
                right->updateRectangle();
                right->parent = (*parent);
            } else {
                left->updateRectangle();
                left->parent = (*parent);
                // right->updateRectangle();
                (*searchNode) = left;
                reInsertNode(right);
            }


            (*parent)->updateRectangle();
        }
        (*searchNode)->updateRectangle();
        return idRegion;
    }

    void reInsertNode(N * node) {
        N ** current = &root;
        N ** previous = &root;
        while ((*current)->rectangle->isInside(*(node->rectangle)) && ( ! (*current)->leaf)) {
            previous = current;
            for (int i = 0; i < (*current)->count; i += 1) {
                current = &(*current)->children[i];
                if ((*current)->rectangle->isInside(*(node->rectangle))) {
                    break;
                }
            }
        }
        int index = (*previous)->addChildren(node);
        node->updateRectangle();
        node->parent = (*previous);

        if (index >= M) {
            N * left = new N(M, id++, false);
            N * right = new N(M, id++, false);
            int a = -1, b = -1;
            (*previous)->getFartherChildren(a, b);

            N ** parent = &((*previous)->parent);
            N * nodeA = (*previous)->children[a];
            N * nodeB = (*previous)->children[b];

            left->addChildren(nodeA);
            nodeA->parent = left;
            right->addChildren(nodeB);
            nodeB->parent = right;

            if (shouldPrintLog) {
                cout << "nodeA " << nodeA->id << endl;
                cout << "nodeB " << nodeB->id << endl;
            }

            for (int i = 0; i < (*previous)->count; i++) {
                if (i == a || i == b) {
                    continue;
                }
                N * currentNode = (*previous)->children[i];
                // cout << "currentNode " << currentNode->id << endl;
                if (nodeA->rectangle->getDistance(*(currentNode->rectangle)) < nodeB->rectangle->getDistance(*(currentNode->rectangle)) && left->count <= m) {
                    left->addChildren(currentNode);
                    currentNode->parent = left;
                } else if (right->count <= m) {
                    right->addChildren(currentNode);
                    currentNode->parent = right;

                    // if (shouldPrintLog) {
                    //     cout << "currentNode " << currentNode->id << endl;
                    //     cout << "parent " << currentNode->parent->id << endl;
                    // }
                } else {
                    left->addChildren(currentNode);
                    currentNode->parent = left;
                }
            }
            left->updateRectangle();

            if (shouldPrintLog) {
                cout << "right size " << right->count << endl;
                for (int i = 0; i < right->count; i += 1) {
                    cout << "id: " << right->children[i]->id << " parent: " << right->children[i]->parent->id << endl;
                }
            }
            if ( ! (*parent) ) {
                (*parent) = new N(M, id++, false);
                (*parent)->addChildren(left);
                (*parent)->addChildren(right);
                right->updateRectangle();
                right->parent = (*parent);
                left->parent = (*parent);
                root = (*parent);
            } else {
                (*previous) = left;
                left->parent = (*parent);
                right->updateRectangle();
                // cout << "*** recursive reinsert ***" << endl;
                // cout << "parent" << (*parent)->id << "|" << (*previous)->id << endl;
                reInsertNode(right);
            }
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
            /*if (shouldPrintLog) {
                cout << "min Distance: " << minDistance << " i: "
            }*/
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
    vector<Polygon<T>> rangeSearch(Node<T>* node,Rectangle<T>* region){
        vector <Node<T>*> childrenNode;
        vector <Polygon<T>> elementsInRange;
        childrenNode = node->getChildrenVector();

        for(auto children : childrenNode){
            if(children->leaf)
            {
                auto polygonsChildren = children->get_vector_polygons();
                for(auto pc : polygonsChildren)
                {
                    if((pc->min.x >= region->min.x && pc->min.y >= region->min.y) &&
                    (pc->max.x <= region->max.x && pc->max.y <= region->max.y)){
                        elementsInRange.push_back(*pc);
                    }
                }

            }
            else{
                if((children->get_rectangle()->min.x >= region->min.x && children->get_rectangle()->min.y >= region->min.y) &&
                (children->get_rectangle()->max.x <= region->max.x && children->get_rectangle()->max.y <= region->max.y)){
                    return rangeSearch(children,region);
                }
            }
        }
        return elementsInRange;
    }
    vector <Polygon<T>> rangeSearch(Rectangle<T>* region){
        vector <Polygon<T>> elementsInRange;
        if(root->leaf){
            auto polygonsRoot = root->get_vector_polygons();
            for(auto pr : polygonsRoot)
            {
                if((pr->min.x >= region->min.x && pr->min.y >= region->min.y) &&
                (pr->max.x <= region->max.x && pr->max.y <= region->max.y)){
                    elementsInRange.push_back(*pr);
                }
            }
            return elementsInRange;
        }
        else{
            return rangeSearch(root,region);
        }
    }
    
    vector <Polygon<T> * > nearestSearch(P point, int k) {
        /*
        - We use 2 arrays:
          a. missingVisits: which store all the nodes that has not been
             visited.
          b. knearest: (answer) The array of nodes which are going to
             be returned.
        - The algorithm stops when the knearest arrays has the k length
          and the last element distance to the point is lower than the
          first node in the missingVisits array.
        - Basically there's 2 cases:
          a. If the node is a leaf, then we add the polygons to the answer
             only if the distance is lower to the last element in the current
             answer
          b. Otherwise, we add the node to the missingVisits
        - At the begining if the root is a leaf, we add directly to
          the missingVisits.
          Otherwise we add the children's root to the missingVisits.
        */
        vector <Node<T> * > missingVisits;
        vector <Polygon<T> * > knearest;

        if (root->leaf) {
            missingVisits.push_back(root);
        } else {
            missingVisits = root->getChildrenVector();
        }

        sort(
            missingVisits.begin(),
            missingVisits.end(),
            PointNodeDistComparison(point)
        );
        while (missingVisits.size() && k > 0) {
            Node <T> * current = missingVisits[0];
            missingVisits.erase(
                missingVisits.begin(), missingVisits.begin() + 1);
            size_t ksize = knearest.size();
            if (knearest.size() == k && (knearest[ksize - 1])->minDist(point) < current->minDist(point)) {
                break;
            }
            if (current->leaf) {
                for(int i = 0; i < current->count; i += 1) {
                    Polygon<T> * polygon = current->polygons[i];
                    int poly_dist = polygon->minDist(point);
                    if (ksize < k) {
                        knearest.push_back(polygon);
                        ksize = knearest.size();
                    }
                    else if (poly_dist < (knearest[ksize - 1])->minDist(point)) {
                        knearest[ksize - 1] = polygon;
                    }
                    sort(
                        knearest.begin(),
                        knearest.end(),
                        PointPolygonDistComparison(point)
                    );
                }
            } else {
                cout << "Not LEAFS" << endl;
                vector <Node<T> * > currentChildren = current->getChildrenVector();
                missingVisits.insert(
                    missingVisits.begin(),
                    currentChildren.begin(),
                    currentChildren.end()
                );
                sort(
                    missingVisits.begin(),
                    missingVisits.end(),
                    PointNodeDistComparison(point)
                );
            }
            cout << "Current knearest" << endl;
            for (int i = 0; i < knearest.size(); i += 1) {
                knearest[i]->print();
            }
        }
        return knearest;
    }

    void print() {
        print(root);
    }

    void get_all(Node<T> *node, vector<Node<T> *> & leafs, vector<Node<T> *> & notleafs) {
        notleafs.push_back(node);
        if (node -> leaf) {
            leafs.push_back(node);
        } else {
            for (int i=0; i < node->count; i++)
            {
                get_all(node->children[i], leafs, notleafs);
            }

        }
        return;
    }

    void get_all(vector<Node<T> *> & leafs, vector<Node<T> *> & notleafs) {
        if ( ! root) {
            return;
        }
        get_all(root, leafs, notleafs);
    }

    string get_json_string() {
        vector<Node<T>* > L;
        vector<Node<T>* > NL;

        get_all(L, NL);

        string json_string = "{\"polygons\": [ ";

        for(auto node : L){
            for (size_t i = 0; i < node->count; i += 1) {
                json_string += "{\"id\":" + std::to_string(
                    node->polygons[i]->id
                ) + ", \"region\":" + to_string(node->rectangle->id) + ",\"polygon\": [";
                for (auto point : node->polygons[i]->points) {
                    json_string += point.to_string();
                }
                json_string.pop_back();
                json_string += "]},";
            }
            json_string.pop_back();
            json_string += ",";
        }
        json_string.pop_back();
        json_string += "], \"regions\": [ ";

        for (auto node : NL) {
            if (node->id == root->id) {
                continue;
            }
            json_string += "{\"id\":" + std::to_string(node->rectangle->id) + ",\"polygon\": [";

            for (auto point: node->get_rectangle()->get_box()) {
                json_string += point.to_string();
            }
            json_string.pop_back();
            json_string += "]},";
        }
        json_string.pop_back();
        json_string += "]}";
        return json_string;
    }
};

#endif // RTREE_H
