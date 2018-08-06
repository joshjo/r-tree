#ifndef RTREE_H
#define RTREE_H

#include "Node.h"

template <class T>
class RTree
{
    private:
        Node<T> *root;
        int minEntries;
        int maxEntries;
        int treeLevel;
        bool firstTime;
        int identifier;

    public:
        RTree(){}
        RTree(int minEntries, int maxEntries)
        {
            this->root = new Node<T>(maxEntries, identifier );
            identifier++;
            this->minEntries = minEntries;
            this->maxEntries = maxEntries;
            this->treeLevel = 0;
            this->firstTime = true;
        }

    void updateRectangle(Node<T> *node, Polygon<T> *polygon)
    {
        do
        {
            if(polygon->x < node->rectangle->minX)
                node->rectangle->minX = polygon->x;

            if(polygon->x > node->rectangle->maxX)
                node->rectangle->maxX = polygon->x;

            if(polygon->y < node->rectangle->minY)
                node->rectangle->minY = polygon->y;

            if(polygon->y > node->rectangle->maxY)
                node->rectangle->maxY = polygon->y;

            node = node->father;
        }
        while(node != NULL);

    }
    void updateRectangleNodes(Node<T> *nodeF, Node<T> *nodeC)
    {
        do
        {
            if(nodeC->rectangle->minX < nodeF->rectangle->minX)
                nodeF->rectangle->minX = nodeC->rectangle->minX;

            if(nodeC->rectangle->maxX > nodeF->rectangle->maxX)
                nodeF->rectangle->maxX = nodeC->rectangle->maxX;

            if(nodeC->rectangle->minY < nodeF->rectangle->minY)
                nodeF->rectangle->minY = nodeC->rectangle->minY;

            if(nodeC->rectangle->maxY > nodeF->rectangle->maxY)
                nodeF->rectangle->maxY = nodeC->rectangle->maxY;

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

        float x1, x2;
        float y1, y2;
        int pos1 = 0, pos2 = 0;

        for(int i=0; i<node->count; i++)
        {
            x1 = node->polygons[i].x;
            y1 = node->polygons[i].y;

            for(int j=i+1; j< node->count; j++)
            {
                x2 = node->polygons[j].x;
                y2 = node->polygons[j].y;

                distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));

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

    void fillExtremeNodes(Node<T> *node, Polygon <T> *polygon, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        Polygon<T> *firstExtremePolygon = new Polygon<T>();
        Polygon<T> *secondExtremePolygon = new Polygon<T>();
        getExtremesPolygons(node, firstExtremePolygon, secondExtremePolygon);

        firstHalfNode->polygons[firstHalfNode->count] = firstExtremePolygon->copy();//*firstExtremePolygon;
        firstHalfNode->count++;
        firstHalfNode->father = node->father;
        updateRectangle(firstHalfNode, firstExtremePolygon);

        secondHalfNode->polygons[secondHalfNode->count] = secondExtremePolygon->copy();//*secondExtremePolygon;
        secondHalfNode->count++;
        secondHalfNode->father = node->father;
        updateRectangle(secondHalfNode, secondExtremePolygon);

    }

    void addNewPolygonToNode(Polygon<T> *polygon, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        float area1 = firstHalfNode->getSimulatedArea(polygon);
        float area2 = secondHalfNode->getSimulatedArea(polygon);

        if(area1 < area2)
        {
            //if((maxEntries-minEntries) >= firstHalfNode->count)
            {
                firstHalfNode->polygons[firstHalfNode->count] = polygon->copy();
                firstHalfNode->count++;
                updateRectangle(firstHalfNode, polygon);
            }
            /*else
            {
                secondHalfNode->polygons[secondHalfNode->count] = polygon->copy();
                secondHalfNode->count++;
                updateRectangle(secondHalfNode, polygon);
            }*/

        }

        else
        {
            //if((maxEntries-minEntries) >= firstHalfNode->count)
            {
                secondHalfNode->polygons[secondHalfNode->count] = polygon->copy();
                secondHalfNode->count++;
                updateRectangle(secondHalfNode, polygon);
            }
            /*else
            {
                firstHalfNode->polygons[firstHalfNode->count] = polygon->copy();
                firstHalfNode->count++;
                updateRectangle(firstHalfNode, polygon);
            }*/
        }
    }

    float getDistance( Polygon<T> * p1, Polygon<T> *p2 )
    {
        return sqrt( pow(p1->x-p2->x,2) + pow(p1->y-p2->y,2));
    }

    void addNewPolygonNearToNode(Node<T> *node, Polygon<T> *polygon, Node<T> *firstHalfNode, Node<T> *secondHalfNode)
    {
        float distance = 0, area1, area2;
        float minDistance = 1000000;
        Polygon<T> *nearPolygon;
        bool firstNode = true;

        for(int i=0; i < node->count; i++)
        {

            if(node->polygons[i].intermediate != true)
            {
                for(int j=0; j<firstHalfNode->count; j++)
                {
                    distance = getDistance( &(firstHalfNode->polygons[0]), &(node->polygons[i]) );

                    if(distance == minDistance)
                    {
                        area1 = firstHalfNode->getSimulatedArea( nearPolygon );
                        area2 = firstHalfNode->getSimulatedArea(&(node->polygons[i]));
                        if(area2<area1)
                        {
                            nearPolygon = &(node->polygons[i]);
                            firstNode = true;
                        }

                    }
                    else if(distance < minDistance)
                    {
                        minDistance = distance;
                        nearPolygon = &(node->polygons[i]);
                        firstNode = true;
                    }


                }

                for(int k=0; k<secondHalfNode->count; k++)
                {
                    distance = getDistance( &(secondHalfNode->polygons[0]), &(node->polygons[i]) );

                    if(distance == minDistance)
                    {
                        area1 = secondHalfNode->getSimulatedArea( nearPolygon );
                        area2 = secondHalfNode->getSimulatedArea(&(node->polygons[i]));
                        if(area2<area1)
                        {
                            nearPolygon = &(node->polygons[i]);
                            firstNode = false;
                        }
                    }
                    else if(distance < minDistance)
                    {
                        minDistance = distance;
                        nearPolygon = &(node->polygons[i]);
                        firstNode = false;
                    }
                }
            }
        }

        if(polygon->intermediate != true)
        {
            distance = getDistance( &(firstHalfNode->polygons[0]), polygon );
            if(distance == minDistance)
            {
                area1 = firstHalfNode->getSimulatedArea( nearPolygon );
                area2 = firstHalfNode->getSimulatedArea( polygon );
                if(area2<area1)
                {
                    nearPolygon = polygon;
                    firstNode = true;
                }

            }
            else if(distance < minDistance)
            {
                minDistance = distance;
                nearPolygon = polygon;
                firstNode = true;
            }

            distance = getDistance( &(secondHalfNode->polygons[0]), polygon );
            if(distance == minDistance)
            {
                area1 = secondHalfNode->getSimulatedArea( nearPolygon );
                area2 = secondHalfNode->getSimulatedArea( polygon );
                if(area2<area1)
                {
                    nearPolygon = polygon;
                    firstNode = false;
                }

            }
            else if(distance < minDistance)
            {
                minDistance = distance;
                nearPolygon = polygon;
                firstNode = false;
            }
        }

        if(firstNode)
        {
            if(firstHalfNode->count < maxEntries-minEntries+1)
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
            if(secondHalfNode->count < maxEntries-minEntries+1)
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
        int remainedNodes = maxEntries -2 +1;


        while(remainedNodes>0)
        {

            addNewPolygonNearToNode(node, polygon, firstHalfNode, secondHalfNode);

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
            if(node->father->count < maxEntries)
            {
                node->father->children[node->father->count] = *newBrotherNode;
                updateRectangleNodes(node->father, &(node->father->children[node->father->count]));
                node->father->count++;
            }
            else
            {
                insertNode(node->father, newBrotherNode);
            }
        }
    }


    void insert(Polygon<T> *polygon)
    {

        if(firstTime)
        {
            root->polygons[root->count] = Polygon<T>(polygon->x, polygon->y, polygon->identifier);
            root->count++;
            firstTime = false;
            updateRectangle(root, polygon);
            return;
        }


        Node<T> *node = search(polygon);//Nodo más cercano

        if(node->count < maxEntries)
        {
            node->polygons[node->count] = polygon->copy();
            node->count++;
            updateRectangle(node, polygon);
        }
        else
        {
            Node<T> *firstHalfNode;
            if(node->father==NULL)
            {
                firstHalfNode = new Node<T>(maxEntries, identifier);
                identifier++;
            }
            else
                firstHalfNode = new Node<T>(maxEntries, node->rectangle->identifier);
            Node<T> *secondHalfNode = new Node<T>(maxEntries, identifier);
            identifier++;

            fillExtremeNodes(node, polygon, firstHalfNode, secondHalfNode);
            //cout<<firstHalfNode->polygons[0].identifier<<endl;
            //cout<<secondHalfNode->polygons[0].identifier<<endl;

            addPolygonsToNode(node, polygon, firstHalfNode, secondHalfNode);
            firstHalfNode->father = node->father;
            *node = *firstHalfNode;

            insertNode(node, secondHalfNode);

            /*node->count = 2;
            node->leaf = false;
            node->children[0]= *firstHalfNode;
            updateRectangleNodes(node, &(node->children[0]) );
            node->children[1]= *secondHalfNode;
            updateRectangleNodes(node, &(node->children[1]) );*/
            //insertarNodoNuevo(node, firstHalfNode, secondHalfNode);



            /*cout<<"======================"<<endl;
            cout<<"first"<<firstHalfNode->count<<endl;
            cout<<"second"<<secondHalfNode->count<<endl;
            cout<<"======================"<<endl;*/
        }


    }



    void print(Node<T> *node)
    {
        cout<<"Identifier rectangle: R"<<node->rectangle->identifier;
        cout<<" x:"<<node->rectangle->minX<<"-"<<node->rectangle->maxX;
        cout<<" y:"<<node->rectangle->minY<<"-"<<node->rectangle->maxY<<endl;
        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                if(node->father != NULL)
                    cout<<"Poly: "<<node->polygons[i].identifier<<" Father: R"<< node->father->rectangle->identifier <<endl;
                else
                    cout<<"Poly: "<<node->polygons[i].identifier <<endl;
            }
        }
        else
        {
            for(int i=0; i<node->count; i++)
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
