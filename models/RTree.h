#ifndef RTREE_H
#define RTREE_H

#include "Node.h"

template <class T>
class RTree
{
    typedef Point<T> P;

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
        Node<T> *root;
        int minEntries; // m
        int maxEntries; // M
        int treeLevel;
        bool firstTime;
        int identifier;

    public:
        RTree(){}
        RTree(int minEntries, int maxEntries)
        {
            this->identifier = 0;
            this->root = new Node<T>(maxEntries, identifier);
            this->identifier++;
            this->minEntries = minEntries;
            this->maxEntries = maxEntries;
            this->treeLevel = 0;
            this->firstTime = true;
        }

    void updateRectangle(Node<T> *node, Polygon<T> *polygon)
    {
        do
        {
            if (polygon->min.x < node->rectangle->min.x){
                node->rectangle->min.x = polygon->min.x;
            }

            if (polygon->max.x > node->rectangle->max.x){
                node->rectangle->max.x = polygon->max.x;
            }

            if (polygon->min.y < node->rectangle->min.y){
                node->rectangle->min.y = polygon->min.y;
            }

            if (polygon->max.y > node->rectangle->max.y){
                node->rectangle->max.y = polygon->max.y;
            }

            node = node->father;
        }
        while(node != NULL);
    }

    void updateRectangleNodes(Node<T> *nodeF, Node<T> *nodeC)
    {
        do
        {
            if(nodeC->rectangle->min.x < nodeF->rectangle->min.x) {
                nodeF->rectangle->min.x = nodeC->rectangle->min.x;
            }
            if(nodeC->rectangle->max.x > nodeF->rectangle->max.x) {
                nodeF->rectangle->max.x = nodeC->rectangle->max.x;
            }

            if(nodeC->rectangle->min.y < nodeF->rectangle->min.y) {
                nodeF->rectangle->min.y = nodeC->rectangle->min.y;
            }

            if(nodeC->rectangle->max.y > nodeF->rectangle->max.y) {
                nodeF->rectangle->max.y = nodeC->rectangle->max.y;
            }
            nodeF = nodeF->father;
        }
        while(nodeF != NULL);
    }

    Node<T>* appropiateLeaf(Node<T>* node, Polygon<T> *polygon)
    {
        if(node->leaf)
            return node;

        float auxArea = 0;
        float minArea = 1000000;
        Node<T> * auxNode;
        for(int i=0; i<node->count; i++)
        {
            auxArea = node->children[i].getSimulatedArea(polygon);

            if(auxArea < minArea)
            {
                minArea = auxArea;
                auxNode = &(node->children[i]);
            }
        }

        return(appropiateLeaf(auxNode, polygon));

    }

    /*
     * Función que busca la hoja más apropiada (cercana) para el nuevo polígono.
    */
    Node<T>* search(Polygon<T> *polygon)
    {
        return(appropiateLeaf(root, polygon));
    }

    void getExtremesPolygons(Node<T> *node, Polygon <T> *firstExtremePolygon, Polygon <T> *secondExtremePolygon)
    {
        float maxDistance = -1000000;
        float distance;

        P p1;
        P p2;
        int pos1 = 0, pos2 = 0;

        for(int i=0; i<node->count; i++)
        {
            //Todo: tranformation in polygon
            p1.x = node->polygons[i].points[0].x;
            p1.y = node->polygons[i].points[0].y;

            for(int j=i+1; j< node->count; j++)
            {
                p2.x = node->polygons[j].points[0].x;
                p2.y = node->polygons[j].points[0].y;

                distance = sqrt(pow((p1.x - p2.x),2) +pow((p1.y - p2.y),2));

                if(distance > maxDistance)
                {
                    maxDistance = distance;
                    *firstExtremePolygon = node->polygons[i];
                    *secondExtremePolygon = node->polygons[j];
                    pos1 = i;
                    pos2 = j;
                }
            }
        }

        node->polygons[pos1].intermediate = true;
        node->polygons[pos2].intermediate = true;
    }

    void fillExtremeNodes(Node<T> *node, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        Polygon<T> *firstExtremePolygon = new Polygon<T>();
        Polygon<T> *secondExtremePolygon = new Polygon<T>();
        getExtremesPolygons(node, firstExtremePolygon, secondExtremePolygon);

        firstHalfNode->polygons[firstHalfNode->count] = firstExtremePolygon->copy();//*firstExtremePolygon;
        firstHalfNode->count++;
//        firstHalfNode->father = node->father;
        updateRectangle(firstHalfNode, firstExtremePolygon);

        secondHalfNode->polygons[secondHalfNode->count] = secondExtremePolygon->copy();//*secondExtremePolygon;
        secondHalfNode->count++;
  //      secondHalfNode->father = node->father;
        updateRectangle(secondHalfNode, secondExtremePolygon);

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

    void addNewPolygonNearToNode(Node<T> *node, Polygon<T> *polygon, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        float distance = 0, area1, area2;
        float minDistance = std::numeric_limits<T>::max();
        Polygon<T> *nearPolygon;
        bool firstNode = true;
        for(int i=0; i < node->count; i++){
            if(node->polygons[i].intermediate != true){
                for(int j=0; j<firstHalfNode->count; j++){
                    distance = getDistance( &(firstHalfNode->polygons[0]), &(node->polygons[i]) );
                    if(distance == minDistance){
                        area1 = firstHalfNode->getSimulatedArea( nearPolygon );
                        area2 = firstHalfNode->getSimulatedArea(&(node->polygons[i]));
                        if(area2 < area1)
                        {
                            nearPolygon = &(node->polygons[i]);
                            firstNode = true;
                        }
                    }
                    else if(distance < minDistance){
                        minDistance = distance;
                        nearPolygon = &(node->polygons[i]);
                        firstNode = true;
                    }
                }
                for(int k=0; k<secondHalfNode->count; k++){
                    distance = getDistance( &(secondHalfNode->polygons[0]), &(node->polygons[i]) );
                    if(distance == minDistance){
                        area1 = secondHalfNode->getSimulatedArea( nearPolygon );
                        area2 = secondHalfNode->getSimulatedArea(&(node->polygons[i]));
                        if(area2<area1){
                            nearPolygon = &(node->polygons[i]);
                            firstNode = false;
                        }
                    }
                    else if(distance < minDistance){
                        minDistance = distance;
                        nearPolygon = &(node->polygons[i]);
                        firstNode = false;
                    }
                }
            }
        }
        if(polygon->intermediate != true){
            distance = getDistance( &(firstHalfNode->polygons[0]), polygon );
            if(distance == minDistance){
                area1 = firstHalfNode->getSimulatedArea( nearPolygon );
                area2 = firstHalfNode->getSimulatedArea( polygon );
                if(area2<area1){
                    nearPolygon = polygon;
                    firstNode = true;
                }
            }
            else if(distance < minDistance){
                minDistance = distance;
                nearPolygon = polygon;
                firstNode = true;
            }
            distance = getDistance( &(secondHalfNode->polygons[0]), polygon );
            if(distance == minDistance){
                area1 = secondHalfNode->getSimulatedArea( nearPolygon );
                area2 = secondHalfNode->getSimulatedArea( polygon );
                if(area2<area1){
                    nearPolygon = polygon;
                    firstNode = false;
                }
            }
            else if(distance < minDistance){
                minDistance = distance;
                nearPolygon = polygon;
                firstNode = false;
            }
        }

        if(firstNode)
        {
            if((firstHalfNode->count) < maxEntries-minEntries+1)
            {
                firstHalfNode->polygons[firstHalfNode->count] = *nearPolygon;
                updateRectangle(firstHalfNode, nearPolygon);
                firstHalfNode->count++;
            }
            else
            {
                secondHalfNode->polygons[secondHalfNode->count] = *nearPolygon;
                updateRectangle(secondHalfNode, nearPolygon);
                secondHalfNode->count++;
            }

        }

        else
        {
            if( (secondHalfNode->count)< maxEntries-minEntries+1)
            {
                secondHalfNode->polygons[secondHalfNode->count] = *nearPolygon;
                updateRectangle(secondHalfNode, nearPolygon);
                secondHalfNode->count++;
            }

            else
            {
                firstHalfNode->polygons[firstHalfNode->count] = *nearPolygon;
                updateRectangle(firstHalfNode, nearPolygon);
                firstHalfNode->count++;
            }
        }
        nearPolygon->intermediate = true;
    }


    void addPolygonsToNode(Node<T> *node, Polygon<T> *polygon, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        int remainedNodes = maxEntries - 2 + 1;


        while (remainedNodes > 0)
        {

            addNewPolygonNearToNode(node, polygon, firstHalfNode, secondHalfNode);

            remainedNodes--;
        }
    }

    void getExtremesNodes(Node<T> *node, Node <T> *firstExtremeNode, Node <T> *secondExtremeNode)
    {
        // Convert this function with class points
        float maxDistance = -1000000;
        float distance;

        float x1, x2;
        float y1, y2;
        int pos1 = 0, pos2 = 0;

        for(int i=0; i<node->count; i++)
        {
            x1 = (node->children[i].rectangle->max.x + node->children[i].rectangle->min.x )/2;
            y1 = (node->children[i].rectangle->max.y + node->children[i].rectangle->min.y )/2;

            for(int j=i+1; j< node->count; j++)
            {
                x2 = (node->children[j].rectangle->max.x + node->children[j].rectangle->min.x )/2;
                y2 = (node->children[j].rectangle->max.y + node->children[j].rectangle->min.y )/2;

                distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));

                if(distance > maxDistance)
                {
                    maxDistance = distance;
                    *firstExtremeNode = node->children[i];
                    *secondExtremeNode = node->children[j];
                    pos1 = i;
                    pos2 = j;
                }
            }
        }

        node->children[pos1].intermediate = true;
        node->children[pos2].intermediate = true;
    }

    void fillExtremeNodesInter(Node<T> *node, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        Node<T> *firstExtremeNode = new Node<T>();
        Node<T> *secondExtremeNode = new Node<T>();
        getExtremesNodes(node, firstExtremeNode, secondExtremeNode);

        firstExtremeNode->father = firstHalfNode;
        firstHalfNode->children[firstHalfNode->count] = *firstExtremeNode;//firstExtremeNode->copy()
        firstHalfNode->count++;
        //firstHalfNode->father = node->father;
        updateRectangleNodes(firstHalfNode, firstExtremeNode);

        secondExtremeNode->father = secondHalfNode;
        secondHalfNode->children[secondHalfNode->count] = *secondExtremeNode;//*secondExtremePolygon;
        secondHalfNode->count++;
        //secondHalfNode->father = node->father;
        updateRectangleNodes(secondHalfNode, secondExtremeNode);

        for(int i=0; i<firstHalfNode->count; i++)
            firstHalfNode->children[i].intermediate = false;

        for(int i=0; i<secondHalfNode->count; i++)
            secondHalfNode->children[i].intermediate = false;
    }

    void addNewNodeNearToNode(
        Node<T> *node,
        Node<T> *newBrotherNode,
        Node<T> *firstHalfNode,
        Node<T> *secondHalfNode
    ) {
        // Convert
        float area = 0;
        float minArea = 1000000;
        Node<T> *nearNode;
        bool firstNode = true;

        for(int i=0; i < node->count; i++)
        {

            if(node->children[i].intermediate != true)
            {
                for(int j=0; j<firstHalfNode->count; j++)
                {
                    area = firstHalfNode->getSimulatedArea( &(node->children[i]) );

                    if(area < minArea)
                    {
                        minArea = area;
                        nearNode = &(node->children[i]);
                        firstNode = true;
                    }
                }

                for(int k=0; k<secondHalfNode->count; k++)
                {
                    area = secondHalfNode->getSimulatedArea( &(node->children[i]) );

                    if(area < minArea)
                    {
                        minArea = area;
                        nearNode = &(node->children[i]);
                        firstNode = false;
                    }
                }
            }
        }

        if(newBrotherNode->intermediate != true)
        {
            area = firstHalfNode->getSimulatedArea( newBrotherNode );
            if(area < minArea)
            {
                minArea = area;
                nearNode = newBrotherNode;
                firstNode = true;
            }

            area = secondHalfNode->getSimulatedArea( newBrotherNode );
            if(area < minArea)
            {
                minArea = area;
                nearNode = newBrotherNode;
                firstNode = false;
            }
        }

        if(firstNode)
        {
            if( (firstHalfNode->count) < maxEntries-minEntries+1)
            {
                nearNode->father = firstHalfNode;
                firstHalfNode->children[firstHalfNode->count] = *nearNode;
                updateRectangleNodes(firstHalfNode, nearNode);
                firstHalfNode->count++;
            }
            else
            {
                nearNode->father = secondHalfNode;
                secondHalfNode->children[secondHalfNode->count] = *nearNode;
                updateRectangleNodes(secondHalfNode, nearNode);
                secondHalfNode->count++;
            }

        }

        else
        {
            if( (secondHalfNode->count) < maxEntries-minEntries+1)
            {
                nearNode->father = secondHalfNode;
                secondHalfNode->children[secondHalfNode->count] = *nearNode;
                updateRectangleNodes(secondHalfNode, nearNode);
                secondHalfNode->count++;
            }

            else
            {
                nearNode->father = firstHalfNode;
                firstHalfNode->children[firstHalfNode->count] = *nearNode;
                updateRectangleNodes(firstHalfNode, nearNode);
                firstHalfNode->count++;
            }
        }
        nearNode->intermediate = true;
    }

    void addPolygonsToNodeInter(Node<T> *node, Node<T> *newBrotherNode, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        int remainedNodes = maxEntries -2 +1;

        while(remainedNodes>0)
        {
            addNewNodeNearToNode(node, newBrotherNode, firstHalfNode, secondHalfNode);
            remainedNodes--;
        }
    }

    void insertNode(Node<T> *node, Node<T> *newBrotherNode)
    {
        if(node->father == NULL)//significa que llegue al root
        {
            Node<T> *newRoot = new Node<T>(maxEntries, 0);
            newRoot->leaf = false;

            node->father = newRoot;
            newRoot->children[newRoot->count]= *node;
            updateRectangleNodes(newRoot, &(newRoot->children[newRoot->count]) );
            newRoot->count++;

            newBrotherNode->father = newRoot;
            newRoot->children[newRoot->count]= *newBrotherNode;
            updateRectangleNodes(newRoot, &(newRoot->children[newRoot->count]) );
            newRoot->count++;

            root = newRoot; // A CONSIDERAR PUEDE SER *root = *newRoot
        }

        else
        {
            if((node->father->count) < maxEntries)
            {
                node->father->children[node->father->count] = *newBrotherNode;
                updateRectangleNodes(node->father, &(node->father->children[node->father->count]));
                node->father->count++;
            }
            else
            {
                Node<T> *firstHalfNode;
                Node<T> *secondHalfNode;

                firstHalfNode = new Node<T>(maxEntries, identifier);
                firstHalfNode->leaf = false;
                secondHalfNode = new Node<T>(maxEntries, identifier);
                secondHalfNode->leaf = false;
                identifier++;


                fillExtremeNodesInter(node->father, firstHalfNode, secondHalfNode);
                addPolygonsToNodeInter(node->father, newBrotherNode, firstHalfNode, secondHalfNode);

                firstHalfNode->father = node->father->father;
                *(node->father) = *firstHalfNode;

                insertNode(node->father, secondHalfNode);
            }
        }
    }

    void insert(Polygon<T> *polygon)
    {
        if(firstTime)
        {
            root->polygons[root->count] = polygon->copy();
            // root->polygons[root->count].print();
            // cout << "firsTime" << endl;
            root->count++;
            firstTime = false;
            updateRectangle(root, polygon);
            return;
        }


        Node<T> *node = search(polygon);//Nodo más cercano

        if ((node->count) < maxEntries)
        {
            node->polygons[node->count] = polygon->copy();
            node->count++;
            updateRectangle(node, polygon);
        }
        else
        {
            Node<T> *firstHalfNode;
            Node<T> *secondHalfNode;

            firstHalfNode = new Node<T>(maxEntries, node->rectangle->identifier);
            secondHalfNode = new Node<T>(maxEntries, identifier);
            identifier++;


            fillExtremeNodes(node, firstHalfNode, secondHalfNode);

            addPolygonsToNode(node, polygon, firstHalfNode, secondHalfNode);
            firstHalfNode->father = node->father;
            *node = *firstHalfNode;

            insertNode(node, secondHalfNode);
        }
    }

    void get_all(Node<T> *node, vector<Node<T> *> & leafs, vector<Node<T> *> & notleafs) {
        notleafs.push_back(node);
        if (node -> leaf) {
            leafs.push_back(node);
        } else {
            for (int i=0; i < node->count; i++)
            {
                get_all(&node->children[i], leafs, notleafs);
            }

        }
        return;
    }

    void get_all(vector<Node<T> *> & leafs, vector<Node<T> *> & notleafs) {
        if (firstTime) {
            return;
        }
        get_all(root, leafs, notleafs);
    }


    vector <Polygon<T> * > nearestSearch(P point, size_t k) {
        vector <Node<T> * > missingVisits = root->getChildrenVector();
        vector <Polygon<T> * > knearest;

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
                    Polygon<T> * polygon = &(current->polygons[i]);
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

    string get_json_string() {
        vector<Node<T>* > L;
        vector<Node<T>* > NL;

        get_all(L, NL);

        string json_string = "{\"polygon\": [ ";

        for(auto node : L){
            json_string += "{\"id\":" + std::to_string(node->polygons->get_id()) + ",\"polygon\": [";
            for (size_t i = 0; i < node->count; i += 1) {
                for (auto point : node->get_polygons()[i].get_points()) {
                    json_string += point.to_string();
                }
            }
            // for (auto point : node->get_polygons()->get_points()) {
            //     json_string += point.to_string();
            // }
            json_string.pop_back();
            json_string += "]},";
        }

        json_string.pop_back();

        json_string += "], \"regions\": [ ";

        for (auto node : NL) {
            json_string += "{\"id\":" + std::to_string(node->rectangle->get_id()) + ",\"polygon\": [";

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

    void print(Node<T> *node)
    {
        cout<<"Identifier rectangle: R"<<node->rectangle->identifier;
        cout<<" x:"<<node->rectangle->min.x<<"-"<<node->rectangle->max.x;
        cout<<" y:"<<node->rectangle->min.y<<"-"<<node->rectangle->max.y<<endl;
        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                if(node->father != NULL)
                    cout<< "Poly: "<<node->polygons[i].identifier<<" Father: R"<< node->father->rectangle->identifier <<endl;
                else
                    cout<< "Poly: "<<node->polygons[i].identifier <<endl;
            }
        }
        else
        {
            for(int i=0; i < node->count; i++)
            {
                print(&node->children[i]);
            }
        }
    }

    void print()
    {
        print(root);
    }

};

#endif // RTREE_H
