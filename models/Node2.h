#ifndef RTREE_H
#define RTREE_H

#include "Node.h"

template <class T>
class RTree
{
    //private:
    public:
        Nodee<T> *root;
        int minEntries;
        int maxEntries;
        int treeLevel;
        bool firstTime;
        int identifier;

    //public:
        RTree(){}
        RTree(int minEntries, int maxEntries)
        {
            this->identifier = 0;
            this->root = new Nodee<T>(maxEntries, identifier );
            identifier++;
            this->minEntries = minEntries;
            this->maxEntries = maxEntries;
            this->treeLevel = 0;
            this->firstTime = true;
        }


    void updateRectangle(Nodee<T> *node, Polygon<T> *polygon)
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
    void updateRectangleNodes(Nodee<T> *nodeF, Nodee<T> *nodeC)
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

    Nodee<T>* appropiateLeaf(Nodee<T>* node, Polygon<T> *polygon)
    {
        if(node->leaf)
            return node;

        float auxArea = 0;
        float minArea = 1000000;//crecimiento adicional
        Nodee<T> * auxNode;
        for(int i=0; i<node->count; i++)
        {
            auxArea = node->children[i].getSimulatedArea(polygon)-node->children[i].getArea();

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
    Nodee<T>* search(Polygon<T> *polygon)
    {
        return(appropiateLeaf(root, polygon));
    }

    void getExtremesPolygons(Nodee<T> *node, Polygon <T> *firstExtremePolygon, Polygon <T> *secondExtremePolygon)
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
                    *firstExtremePolygon = (node->polygons[i]);
                    *secondExtremePolygon = (node->polygons[j]);
                    pos1 = i;
                    pos2 = j;
                }
            }
        }

        node->polygons[pos1].intermediate = true;
        node->polygons[pos2].intermediate = true;
    }

    void fillExtremeNodes(Nodee<T> *node, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
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
        return sqrt( pow(p1->x-p2->x,2) + pow(p1->y-p2->y,2));
    }
    void addNewPolygonNearToNode(Nodee<T> *node, Polygon<T> *polygon, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
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
            if( (firstHalfNode->count) < maxEntries-minEntries+1)
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


    void addPolygonsToNode(Nodee<T> *node, Polygon<T> *polygon, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        int remainedNodes = maxEntries -2 +1;


        while(remainedNodes>0)
        {

            addNewPolygonNearToNode(node, polygon, firstHalfNode, secondHalfNode);

            remainedNodes--;
        }
    }








    //=========================================================

     void getExtremesNodes(Nodee<T> *node, Nodee <T> *firstExtremeNode, Nodee <T> *secondExtremeNode)
    {
        float maxDistance = -1000000;
        float distance;

        float x1, x2;
        float y1, y2;
        int pos1 = 0, pos2 = 0;

        for(int i=0; i<node->count; i++)
        {
            x1 = (node->children[i].rectangle->maxX + node->children[i].rectangle->minX )/2;
            y1 = (node->children[i].rectangle->maxY + node->children[i].rectangle->minY )/2;

            for(int j=i+1; j< node->count; j++)
            {
                x2 = (node->children[j].rectangle->maxX + node->children[j].rectangle->minX )/2;
                y2 = (node->children[j].rectangle->maxY + node->children[j].rectangle->minY )/2;

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



    void fillExtremeNodesInter(Nodee<T> *node, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        Nodee<T> *firstExtremeNode = new Nodee<T>();
        Nodee<T> *secondExtremeNode = new Nodee<T>();
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




    void addNewNodeNearToNode(Nodee<T> *node, Nodee<T> *newBrotherNode, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        float area = 0;
        float minArea = 1000000;
        Nodee<T> *nearNode;
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



    void addPolygonsToNodeInter(Nodee<T> *node, Nodee<T> *newBrotherNode, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        int remainedNodes = maxEntries -2 +1;


        while(remainedNodes>0)
        {
            addNewNodeNearToNode(node, newBrotherNode, firstHalfNode, secondHalfNode);
            remainedNodes--;
        }
    }


    //================================================================









    void insertNode(Nodee<T> *node, Nodee<T> *newBrotherNode)
    {
        if(node->father == NULL)//significa que llegue al root
        {
            Nodee<T> *newRoot = new Nodee<T>(maxEntries, 0);
            newRoot->leaf = false;

            node->father = newRoot;
            newRoot->children[newRoot->count]= *node;
            updateRectangleNodes(newRoot, &(newRoot->children[newRoot->count]) );
            newRoot->children[newRoot->count].rectangle->identifier = identifier;
            identifier++;
            newRoot->count++;

            newBrotherNode->father = newRoot;
            newRoot->children[newRoot->count]= *newBrotherNode;
            updateRectangleNodes(newRoot, &(newRoot->children[newRoot->count]) );
            newRoot->count++;


            root = newRoot; // A CONSIDERAR PUEDE SER *root = *newRoot
        }

        else
        {
            if( (node->father->count) < maxEntries)
            {
                newBrotherNode->father = node->father;
                node->father->children[node->father->count] = *newBrotherNode;
                updateRectangleNodes(node->father, &(node->father->children[node->father->count]));
                node->father->count++;
            }
            else
            {
                Nodee<T> *firstHalfNode;
                Nodee<T> *secondHalfNode;

                firstHalfNode = new Nodee<T>(maxEntries, identifier);
                firstHalfNode->leaf = false;
                secondHalfNode = new Nodee<T>(maxEntries, identifier);
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
            root->polygons[root->count] = Polygon<T>(polygon->x, polygon->y, polygon->identifier);
            root->count++;
            firstTime = false;
            updateRectangle(root, polygon);
            return;
        }


        Nodee<T> *node = search(polygon);//Nodo más cercano

        if( (node->count) < maxEntries)
        {
            node->polygons[node->count] = polygon->copy();
            node->count++;
            updateRectangle(node, polygon);
        }
        else
        {
            Nodee<T> *firstHalfNode;
            Nodee<T> *secondHalfNode;

            firstHalfNode = new Nodee<T>(maxEntries, node->rectangle->identifier);
            secondHalfNode = new Nodee<T>(maxEntries, identifier);
            identifier++;


            fillExtremeNodes(node, firstHalfNode, secondHalfNode);

            addPolygonsToNode(node, polygon, firstHalfNode, secondHalfNode);
            firstHalfNode->father = node->father;
            *node = *firstHalfNode;

            insertNode(node, secondHalfNode);
        }

        //paint();
    }



    void print(Nodee<T> *node, int nivel)
    {
        string tabs= "";
        for(int i=0; i<nivel; i++)
            tabs = tabs + "\t";
        cout<<tabs<<"Rectangle: R"<<node->rectangle->identifier;
        cout<<" x:"<<node->rectangle->minX<<"-"<<node->rectangle->maxX;
        cout<<" y:"<<node->rectangle->minY<<"-"<<node->rectangle->maxY;
        if(node->father != NULL)
            cout<<" Father: R"<< node->father->rectangle->identifier;

        cout<<endl;


        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                if(node->father != NULL)
                    cout<<tabs<<"Poly: "<<node->polygons[i].identifier<<" Father: R"<< node->father->rectangle->identifier <<endl;
                else
                    cout<<tabs<<"Poly: "<<node->polygons[i].identifier <<endl;
            }
        }
        else
        {
            for(int i=0; i<node->count; i++)
            {
                print(&node->children[i], nivel+1);
            }
        }
    }

    vector <Scalar> colores;
    void print()
    {
        print(root, 0);
    }

    void paint(Nodee<T> *node, int nivel, Mat &img)
    {
        float x1 = node->rectangle->minX*30;
        float x2 = node->rectangle->maxX*30;
        float y1 = 600-node->rectangle->minY*30;
        float y2 = 600-node->rectangle->maxY*30;
        rectangle(img, Point2f(x1, y1), Point2f(x2, y2), colores[nivel], 6-nivel, 8, 0);
        putText(img, "R"+to_string(node->rectangle->identifier) , Point2f(x1+6,y2-6), FONT_ITALIC, 1-0.1*nivel,  colores[nivel], 2 , 8 , false);


        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                x1 = node->polygons[i].x*30;
                y1 = 600-node->polygons[i].y*30;
                circle(img, Point2f(x1, y1), 6, Scalar(255, 255, 255), -1, 8, 0);
                putText(img, node->polygons[i].identifier , Point2f(x1-15,y1+20), FONT_ITALIC, 0.5,  Scalar(255, 255, 255), 2 , 8 , false);
            }
        }
        else
        {
            for(int i=0; i<node->count; i++)
            {
                paint(&node->children[i], nivel+1, img);
            }
        }
    }
    void paint()
    {
        colores.push_back(Scalar(0,   0,   200));
        colores.push_back(Scalar(0,   200, 200));
        colores.push_back(Scalar(0,   200, 0  ));
        colores.push_back(Scalar(200, 200, 0  ));
        colores.push_back(Scalar(200, 0  , 0  ));
        colores.push_back(Scalar(200, 0  , 200));

        Mat img(600, 600, CV_8UC3, Scalar(0,0,0));
        paint(root, 0, img);
        imshow("R0", img);
        waitKey(0);
    }
};

#endif // RTREE_H
