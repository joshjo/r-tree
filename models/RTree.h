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
        if (polygon->id == 10) {
            shouldPrintLog = true;
        } else {
            shouldPrintLog = false;
        }
        N ** searchNode = search(polygon);
        if ( ! (*searchNode)) {
            (*searchNode) = new N (M, id++);
        }

        int index = (*searchNode)->addPolygon(polygon);
        if (index >= M) {
            int a = -1, b = -1;
            (*searchNode)->getFartherPolygons(a, b);

            N * left = new N(M, (*searchNode)->id, true, (*searchNode)->color);
            N * right = new N(M, id++);
            Poly * polygonA = (*searchNode)->polygons[a];
            Poly * polygonB = (*searchNode)->polygons[b];

            left->addPolygon(polygonA);
            right->addPolygon(polygonB);

            for (int i = 0; i < (*searchNode)->count; i++) {
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

            N ** parent = &((*searchNode)->parent);

            if ( ! (*parent)) {
                (*parent) = new N(M, 0, false);
                delete root;
                (*parent)->addChildren(left);
                root = (*parent);
            } else {
                // delete (*searchNode);
                (*searchNode) = left;
            }

            index = (*parent)->addChildren(right);
            left->updateRectangle();
            left->parent = (*parent);
            right->updateRectangle();
            right->parent = (*parent);
            (*parent)->updateRectangle();
            reorderParent(*parent);
        }
        (*searchNode)->updateRectangle(true);

        if (root) {
            root->updateRectangle();
        }
        return id - 1;
    }

    void reorderParent (N* & node) {
        if (node == NULL || node->count <= M) {
            return;
        }
        int leftId = (node->id > 0) ? node->id : id++;
        string color = (node->id > 0) ? "" : node->color;
        N * left = new N(M, leftId, false, color);
        N * right = new N(M, id++, false);

        int a = -1, b = -1;
        node->getFartherChildren(a, b);
        N * nodeA = node->children[a];
        N * nodeB = node->children[b];
        N * parent = node->parent;

        nodeA->parent = left;
        nodeB->parent = right;

        left->addChildren(nodeA);
        right->addChildren(nodeB);

        for (int i = 0; i < node->count; i++) {
            if (i == a || i == b) {
                continue;
            }
            N * currentNode = node->children[i];
            if (nodeA->rectangle->getDistance(*(currentNode->rectangle)) < nodeB->rectangle->getDistance(*(currentNode->rectangle)) && left->count <= m) {
                left->addChildren(currentNode);
                currentNode->parent = left;
            } else if (right->count <= m) {
                right->addChildren(currentNode);
                currentNode->parent = right;
            } else {
                left->addChildren(currentNode);
                currentNode->parent = left;
            }
        }

        left->updateRectangle();
        right->updateRectangle();

        if (!parent) {
            N* newParent = new N(M, 0, false);
            newParent->addChildren(left);
            newParent->addChildren(right);

            left->parent = newParent;
            right->parent = newParent;

            root = newParent;
            newParent->updateRectangle();
        }
        else {
            left->parent = parent;
            right->parent = parent;
            for (int i = 0; i < parent->count; i+= 1) {
                if (parent->children[i] == node ) {
                    parent->children[i] = left;
                    break;
                }
            }
            parent->addChildren(right);
            parent->updateRectangle();
            reorderParent(parent);
        }
    }

    N ** search(Polygon<T> * polygon) {
        if (! root) {
            return &root;
        }
        N ** current = &root;
        //N ** previous = &root;

        vector<N **> zerosArea;
        while (current) {
            N * node = (*current);
            if (node->leaf) {
                break;
            }
            float minDistance = std::numeric_limits<T>::max();
            int index = -1;
            for (int i = 0; i < node->count; i += 1) {
                N * children = node->children[i];
                float oldArea = children->rectangle->getArea();
                float newArea = children->rectangle->getSimulatedArea(polygon);
                float newDistance = newArea - oldArea;
                if (newDistance < minDistance) {
                    index = i;
                    minDistance = newDistance;
                }
                if ( ! newDistance && children->leaf) {
                    zerosArea.push_back(&(node->children[i]));
                }
            }
            current = &(node->children[index]);
        }
        if (zerosArea.size() > 1) {
            T minArea = inf;
            size_t index;
            for (size_t i = 0; i < zerosArea.size(); i += 1) {
                T deadArea = (*(zerosArea[i]))->deadArea();
                if (deadArea < minArea) {
                    minArea = deadArea;
                    index = i;
                }
            }
            current = zerosArea[index];
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
        if (node->parent != NULL) {
            cout <<"["<< node->parent->rectangle->id << "(" << node->parent << ")"  << "] -> ";
        }
        cout<<"R"<<node->rectangle->id << "(" << node << ") ";
        cout<<" x:"<<node->rectangle->min.x<<"-"<<node->rectangle->max.x;
        cout<<" y:"<<node->rectangle->min.y<<"-"<<node->rectangle->max.y<<endl;

        if (node->leaf) {
            for (int i=0; i<node->count; i++) {
                cout<< "    Poly: "<< node->polygons[i]->id << " <=> " << node->polygons[i]->points[0].to_string() <<endl;
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

    void graphviz(Node<T> *node, string & tree){
        // tree += to_string(node->rectangle->id);
        tree += node->rectangle->get_strid() + " [ label =\"" + node->rectangle->get_strid() + " | " + node->rectangle->min.to_string() + " | " + node->rectangle->max.to_string() + "\" shape = \"record\"  ]\n";
        if (node->parent != NULL) {
            tree += node->parent->rectangle->get_strid() + " -> " + node->rectangle->get_strid() + "\n";
        }
        if (node->leaf) {
            // tree += node->parent->rectangle->get_strid() + " [ label =\"" + "x" + "\" shape = \"record\"  ]\n";
            string polygons;
            for (int i=0; i<node->count; i++) {
                tree += node->rectangle->get_strid() + " -> " + node->polygons[i]->get_strid() + "\n";
                polygons += node->polygons[i]->get_strid() + " [ ";
                polygons += "label = \"" + node->polygons[i]->get_strid() + " | " + node->polygons[i]->points[0].to_string() + "\" shape = \"record\" color = \"blue\" ]\n";
            }
            tree += polygons;
        } else {
            // cout << "heeeere" << endl;
            for (int i=0; i < node->count; i++) {
                graphviz(node->children[i], tree);
            }
        }
    }

    string graphviz(){
        string str = "digraph G {\n";
        string tree = "";
        graphviz(root, tree);
        str += tree + "}";
        return str;
    }

    vector <Polygon<T> * > nearestSearch(P point, size_t k) {
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
        if ( ! root) {
            return knearest;
        }
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
            for (size_t i = 0; i < knearest.size(); i += 1) {
                knearest[i]->print();
            }
        }
        return knearest;
    }

    vector<Polygon<T>> rangeSearch(Node<T>* node,Rectangle<T>* region,vector <Polygon<T> > &elementsInRange){
        vector <Node<T>*> childrenNode;
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
                    T l,b,r,t,l1,b1,r1,t1,l2,b2,r2,t2;
                    l1 = children->get_rectangle()->min.x;
                    l2 = region->min.x;
                    b1 = children->get_rectangle()->min.y;
                    b2 = region->min.y;
                    r1 = children->get_rectangle()->max.x;
                    r2 = region->max.x;
                    t1 = children->get_rectangle()->max.y;
                    t2 = region->max.y;

                    if(l1 > l2)
                        l = l1;
                    else
                        l = l2;
                    if(b1 > b2)
                        b = b1;
                    else
                        b = b2;
                    if(r1 < r2)
                        r = r1;
                    else
                        r = r2;
                    if(t1 < t2)
                        t = t1;
                    else
                        t = t2;

                    if(((region->min.x <= l && region->min.y <= b) ||
                    (region->max.x >= r && region->max.y >= t)) && (r > l && t > b) )
                    {
                        rangeSearch(children,region,elementsInRange);
                    }
            }
        }
        return elementsInRange;
    }

    vector <Polygon<T> > rangeSearch(Rectangle<T>* region){
        vector <Polygon<T> > elementsInRange;
        if (! root) {
            return elementsInRange;
        }
        if (root->leaf) {
            auto polygonsRoot = root->get_vector_polygons();
            for (auto pr : polygonsRoot) {
                if((pr->min.x >= region->min.x && pr->min.y >= region->min.y) &&
                (pr->max.x <= region->max.x && pr->max.y <= region->max.y)) {
                    elementsInRange.push_back(*pr);
                }
            }
            return elementsInRange;
        } else {
            return rangeSearch(root, region,elementsInRange);
        }
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
            for (int i = 0; i < node->count; i += 1) {
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



        for (auto node : L) {
            if (!root) {
                continue;
            }
            json_string += "{\"id\":" + std::to_string(node->rectangle->id) + ",\"isLeaf\": true" + ",\"color\":\"" + node->color + "\",\"polygon\": [";

            for (auto point: node->get_rectangle()->get_box()) {
                json_string += point.to_string();
            }
            json_string.pop_back();
            json_string += "]},";
        }
        
        for (auto node : NL) {
            if (!root) {
                continue;
            }
            if (node->leaf) {
                continue;
            }
            json_string += "{\"id\":" + std::to_string(node->rectangle->id) + ",\"isLeaf\": false" + ",\"color\":\"" + node->color + "\",\"polygon\": [";

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
