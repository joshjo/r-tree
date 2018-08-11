#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

template <class T> class RTree;
template <class T> class Nodee;
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
    friend class Nodee<T>;
};

template <class T>
class Polygon
{
    private:
        float x;
        float y;
        bool intermediate;
        T identifier;

    public:
        Polygon(){}
        Polygon(float x, float y, T identifier)
        {
            this->x = x;
            this->y = y;
            this->intermediate = false;
            this->identifier = identifier;
        }
        
        Polygon<T> copy()
        {
            return Polygon(x, y, identifier);
        }
    friend class RTree<T>;
    friend class Nodee<T>;
};

template <class T>
class Nodee
{
    private:
        int count;
        int maxEntries;
        Rectangle<T> *rectangle;
        Nodee<T> **children;
        Nodee<T> *father;
        Polygon<T> *polygons;
        bool leaf;
        bool intermediate;

    public:
        Nodee(){}
        Nodee(int maxEntries, int identifier)
        {
            this->count = 0;
            this->maxEntries = maxEntries;
            this->rectangle = new Rectangle<T>(1000000,-1000000,1000000,-1000000, identifier);
            this->children = new Nodee<T>*[maxEntries];
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
    
    float getSimulatedArea(Nodee<T> * node)
    {
        float minX = rectangle->minX;
        float maxX = rectangle->maxX;
        float minY = rectangle->minY;
        float maxY = rectangle->maxY;
        
        if(node->rectangle->minX < rectangle->minX)
            rectangle->minX = node->rectangle->minX;

        if(node->rectangle->maxX > rectangle->maxX)
            rectangle->maxX = node->rectangle->maxX;

        if(node->rectangle->minY < rectangle->minY)
            rectangle->minY = node->rectangle->minY;

        if(node->rectangle->maxY > rectangle->maxY)
            rectangle->maxY = node->rectangle->maxY;
        
        float area = getArea();
        
        rectangle->minX = minX;
        rectangle->maxX = maxX;
        rectangle->minY = minY;
        rectangle->maxY = maxY;
        
        return area;
    }
    
    
    
    friend class RTree<T>;
};

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
            auxArea = node->children[i]->getSimulatedArea(polygon)-node->children[i]->getArea();
            
            if(auxArea < minArea)
            {
                minArea = auxArea;
                auxNode = node->children[i];
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
        updateRectangle(firstHalfNode, firstExtremePolygon);
        
        secondHalfNode->polygons[secondHalfNode->count] = secondExtremePolygon->copy();//*secondExtremePolygon;
        secondHalfNode->count++;
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
    
     void getExtremesNodes(Nodee<T> *node, Nodee <T> *&firstExtremeNode, Nodee <T> *&secondExtremeNode)
    {
        float maxDistance = -1000000;
        float distance;
        
        float x1, x2;
        float y1, y2;
        int pos1 = 0, pos2 = 0;
        
        for(int i=0; i<node->count; i++)
        {
            x1 = (node->children[i]->rectangle->maxX + node->children[i]->rectangle->minX )/2;
            y1 = (node->children[i]->rectangle->maxY + node->children[i]->rectangle->minY )/2;
            
            for(int j=i+1; j< node->count; j++)
            {
                x2 = (node->children[j]->rectangle->maxX + node->children[j]->rectangle->minX )/2;
                y2 = (node->children[j]->rectangle->maxY + node->children[j]->rectangle->minY )/2;
                
                distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));
                
                if(distance > maxDistance)
                {
                    maxDistance = distance;
                    firstExtremeNode = node->children[i];
                    secondExtremeNode = node->children[j];
                    pos1 = i;
                    pos2 = j;
                    
                }
            }
        }
        
        node->children[pos1]->intermediate = true;
        node->children[pos2]->intermediate = true;
    }
    

    
    void fillExtremeNodesInter(Nodee<T> *node, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        Nodee<T> *firstExtremeNode = new Nodee<T>();
        Nodee<T> *secondExtremeNode = new Nodee<T>();        
        getExtremesNodes(node, firstExtremeNode, secondExtremeNode);
     
        firstExtremeNode->father = firstHalfNode;
        firstHalfNode->children[firstHalfNode->count] = firstExtremeNode;//firstExtremeNode->copy()
        firstHalfNode->count++;
        //firstHalfNode->father = node->father;
        updateRectangleNodes(firstHalfNode, firstExtremeNode);
        
        secondExtremeNode->father = secondHalfNode;
        secondHalfNode->children[secondHalfNode->count] = secondExtremeNode;//*secondExtremePolygon;
        secondHalfNode->count++;
        //secondHalfNode->father = node->father;
        updateRectangleNodes(secondHalfNode, secondExtremeNode);
        
        //for(int i=0; i<firstHalfNode->count; i++)
        //    firstHalfNode->children[i].intermediate = false;
        
        //for(int i=0; i<secondHalfNode->count; i++)
        //    secondHalfNode->children[i].intermediate = false;
    }

    
    
    
    void addNewNodeNearToNode(Nodee<T> *node, Nodee<T> *newBrotherNode, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        float area = 0;
        float minArea = 1000000;
        Nodee<T> *nearNode;
        bool firstNode = true;
        
        for(int i=0; i < node->count; i++)
        {
          
            if(node->children[i]->intermediate != true)
            {
                for(int j=0; j<firstHalfNode->count; j++)
                {
                    area = firstHalfNode->getSimulatedArea( (node->children[i]) ); 
                    
                    if(area < minArea)
                    {
                        minArea = area;
                        nearNode = (node->children[i]);                        
                        firstNode = true;
                    }
                }
            
                for(int k=0; k<secondHalfNode->count; k++)
                {
                    area = secondHalfNode->getSimulatedArea( (node->children[i]) ); 
                    
                    if(area < minArea)
                    {
                        minArea = area;
                        nearNode = (node->children[i]);
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
                firstHalfNode->children[firstHalfNode->count] = nearNode;
                updateRectangleNodes(firstHalfNode, nearNode);
                firstHalfNode->count++;
            }
            else
            {
                nearNode->father = secondHalfNode;
                secondHalfNode->children[secondHalfNode->count] = nearNode;
                updateRectangleNodes(secondHalfNode, nearNode);
                secondHalfNode->count++;
            }
            
        }
        
        else
        {
            if( (secondHalfNode->count) < maxEntries-minEntries+1)
            {
                nearNode->father = secondHalfNode;
                secondHalfNode->children[secondHalfNode->count] = nearNode;
                updateRectangleNodes(secondHalfNode, nearNode);
                secondHalfNode->count++;
            }
            
            else
            {
                nearNode->father = firstHalfNode;
                firstHalfNode->children[firstHalfNode->count] = nearNode;
                updateRectangleNodes(firstHalfNode, nearNode);
                firstHalfNode->count++;
            }
        }
        nearNode->intermediate = true;
    }
    
   
    
    void addPolygonsToNodeInter(Nodee<T> *node, Nodee<T> *newBrotherNode, Nodee<T> *firstHalfNode, Nodee<T> *secondHalfNode)
    {
        int remainedNodes = maxEntries -2 +1;
        
        cout<<"TipoI:"<<newBrotherNode->intermediate<<endl;        
        while(remainedNodes>0)
        {
            addNewNodeNearToNode(node, newBrotherNode, firstHalfNode, secondHalfNode);
            
            remainedNodes--;
        }
        cout<<"TipoF:"<<newBrotherNode->intermediate<<endl;
        for(int i=0; i<firstHalfNode->count; i++)
            firstHalfNode->children[i]->intermediate = false;
        
        for(int i=0; i<secondHalfNode->count; i++)
            secondHalfNode->children[i]->intermediate = false;
    }
    
    
    //================================================================
    
    
    
    
    
    
    
    
    
    void insertNode(Nodee<T> *node, Nodee<T> *newBrotherNode)
    {
        if(node->father == NULL)//significa que llegue al root
        {
            Nodee<T> *newRoot = new Nodee<T>(maxEntries, 0);
            newRoot->leaf = false;
            
            node->father = newRoot;
            newRoot->children[newRoot->count]= node;
            updateRectangleNodes(newRoot, (newRoot->children[newRoot->count]) );
            newRoot->children[newRoot->count]->rectangle->identifier = identifier;
            identifier++;
            newRoot->count++;
            
            newBrotherNode->father = newRoot;
            newRoot->children[newRoot->count]= newBrotherNode;
            updateRectangleNodes(newRoot, (newRoot->children[newRoot->count]) );
            newRoot->count++;
            
            
            root = newRoot; // A CONSIDERAR PUEDE SER *root = *newRoot
        }
        
        else
        {     
            if( (node->father->count) < maxEntries)
            {
                newBrotherNode->father = node->father;
                node->father->children[node->father->count] = newBrotherNode;
                updateRectangleNodes(node->father, (node->father->children[node->father->count]));
                node->father->count++;
                
            }
            else
            {
                Nodee<T> *firstHalfNode;
                Nodee<T> *secondHalfNode;

                firstHalfNode = new Nodee<T>(maxEntries, node->rectangle->identifier);
                firstHalfNode->leaf = false;
                secondHalfNode = new Nodee<T>(maxEntries, identifier);
                secondHalfNode->leaf = false;
                identifier++;

                firstHalfNode->father = node->father->father;
                
                fillExtremeNodesInter(node->father, firstHalfNode, secondHalfNode);
                addPolygonsToNodeInter(node->father, newBrotherNode, firstHalfNode, secondHalfNode);                        
                
                
                cout<<"Ini"<<endl;
                print(firstHalfNode,0);
                cout<<"---"<<endl;
                print(secondHalfNode,0);
                cout<<"Fin"<<endl;
                
                
                
                
                //print(firstHalfNode,0);
                //print(secondHalfNode,0);
                
                
                *(node->father) = *firstHalfNode;
                
                
                
                insertNode(node->father, secondHalfNode);
                cout<<"tam"<<node->father->count<<endl;
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
        cout<<"Mas cercano:"<<node->rectangle->identifier<<endl;

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
            
            firstHalfNode->father = node->father;
            
            fillExtremeNodes(node, firstHalfNode, secondHalfNode);
            addPolygonsToNode(node, polygon, firstHalfNode, secondHalfNode);   
            
            /*cout<<"Ini"<<endl;
            print(firstHalfNode,0);
            cout<<"--"<<endl;
            print(secondHalfNode,0);
            cout<<"Fin"<<endl;
            */
            
            
            *node = *firstHalfNode;
            
            /*if(node->father != NULL)
            {
                print(node->father,0);
                cout<<"--"<<endl;
                cout<<"Fin"<<endl;
            }*/
            
            insertNode(node, secondHalfNode);
        }
        
    }

    

    void print(Nodee<T> *node, int nivel)
    {
        string tabs= "";
        for(int i=0; i<nivel; i++)
            tabs = tabs + "\t";
        cout<<tabs<<"Rectangle: R"<<node->rectangle->identifier;
        cout<<" x:"<<node->rectangle->minX<<"-"<<node->rectangle->maxX;
        cout<<" y:"<<node->rectangle->minY<<"-"<<node->rectangle->maxY;
        
        cout<< "Cant: "<<node->count<<endl;
        
            
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
                print(node->children[i], nivel+1);
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
        int scale = 1;
        float x1 = node->rectangle->minX*scale;
        float x2 = node->rectangle->maxX*scale;
        float y1 = 700-node->rectangle->minY*scale;
        float y2 = 700-node->rectangle->maxY*scale;
        rectangle(img, Point2f(x1, y1), Point2f(x2, y2), colores[nivel], 6-nivel, 8, 0);
        putText(img, "R"+to_string(node->rectangle->identifier) , Point2f(x1+6,y2-6), FONT_ITALIC, 1-0.1*nivel,  colores[nivel], 2 , 8 , false);

        
        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                x1 = node->polygons[i].x*scale;
                y1 = 700-node->polygons[i].y*scale;
                circle(img, Point2f(x1, y1), 6, Scalar(255, 255, 255), -1, 8, 0);
                putText(img, node->polygons[i].identifier , Point2f(x1-15,y1+20), FONT_ITALIC, 0.5,  Scalar(255, 255, 255), 2 , 8 , false);
            }
        }
        else
        {
            for(int i=0; i<node->count; i++)
            {
                paint(node->children[i], nivel+1, img);
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
        
        //Tamaño de imagen
        Mat img(700, 1400, CV_8UC3, Scalar(0,0,0));
        paint(root, 0, img);
        imshow("R0", img);        
        waitKey(0);
    }
};


class Piedra
{
public:
    int id;
    string name;
    Piedra * padre;
    Piedra (int id, string name)
    {
        this->id = id;
        this->name = name;
    }
};
int main(int argc, char const *argv[])
{
    
    /*RTree<string> *tree = new RTree<string>(2,5);
    
    Polygon<string> *poly = new Polygon<string>(2,3,"A");
    Polygon<string> *poly1 = new Polygon<string>(2,6,"B");
    Polygon<string> *poly2 = new Polygon<string>(3,4,"C");
    Polygon<string> *poly3 = new Polygon<string>(3,1,"D");

    tree->insert(poly);
    tree->insert(poly1);
    tree->insert(poly2);
    tree->insert(poly3);

    Polygon<string> *poly4 = new Polygon<string>(6,9,"E");
    tree->insert(poly4);
    Polygon<string> *poly5 = new Polygon<string>(3,3,"F");
    tree->insert(poly5);
    Polygon<string> *poly6 = new Polygon<string>(9,2,"G");
    tree->insert(poly6);
    
    
    Polygon<string> *poly7 = new Polygon<string>(8,3,"H");
    tree->insert(poly7);
    Polygon<string> *poly8 = new Polygon<string>(9,6,"I");
    tree->insert(poly8);
    Polygon<string> *poly9 = new Polygon<string>(9,6,"J");
    tree->insert(poly9);
    Polygon<string> *poly10 = new Polygon<string>(9,6,"K");
    tree->insert(poly10);
    Polygon<string> *poly11 = new Polygon<string>(9,6,"L");
    tree->insert(poly11);
    Polygon<string> *poly12 = new Polygon<string>(11,8,"M");
    tree->insert(poly12);
    Polygon<string> *poly13 = new Polygon<string>(13,9,"N");
    tree->insert(poly13);
    
    Polygon<string> *poly14 = new Polygon<string>(13,8,"NNs");
    tree->insert(poly14);
    Polygon<string> *poly15 = new Polygon<string>(14,9,"O");
    tree->insert(poly15);
    Polygon<string> *poly16 = new Polygon<string>(15,8,"P");
    tree->insert(poly16);
    
    Polygon<string> *poly17 = new Polygon<string>(15,11,"Q");
    tree->insert(poly17);
    
    Polygon<string> *poly18 = new Polygon<string>(4,8,"R");
    //tree->insert(poly18);

    tree->print();
    tree->paint();*/
    

    //PRIMER CASO
    /*
    RTree<string> *tree = new RTree<string>(2,5);
    
    Polygon<string> *poly = new Polygon<string>(454,205,"A");
    Polygon<string> *poly1 = new Polygon<string>(255,428,"B");
    Polygon<string> *poly2 = new Polygon<string>(806,533,"C");
    Polygon<string> *poly3 = new Polygon<string>(856,346,"D");

    Polygon<string> *poly4 = new Polygon<string>(650,177,"E");
    Polygon<string> *poly5 = new Polygon<string>(598,305,"F");
    Polygon<string> *poly6 = new Polygon<string>(548,467,"G");
    Polygon<string> *poly7 = new Polygon<string>(534,668,"H");

    Polygon<string> *poly8 = new Polygon<string>(1262,405,"I");
    Polygon<string> *poly9 = new Polygon<string>(751,308,"J");
    Polygon<string> *poly10 = new Polygon<string>(794,441,"K");
    Polygon<string> *poly11 = new Polygon<string>(745,441,"L");

    Polygon<string> *poly12 = new Polygon<string>(702,406,"M");
    Polygon<string> *poly13 = new Polygon<string>(573,403,"N");
    Polygon<string> *poly14 = new Polygon<string>(530,420,"O");
    Polygon<string> *poly15 = new Polygon<string>(560,512,"P");

    Polygon<string> *poly16 = new Polygon<string>(673,601,"Q");
    Polygon<string> *poly17 = new Polygon<string>(917,323,"R");
    Polygon<string> *poly18 = new Polygon<string>(487,313,"S");


    tree->insert(poly);
    tree->insert(poly1);
    tree->insert(poly2);
    tree->insert(poly3);
    tree->insert(poly4);
    tree->insert(poly5);
    tree->insert(poly6);
    tree->insert(poly7);
    tree->insert(poly8);
    tree->insert(poly9);
    tree->insert(poly10);
    tree->insert(poly11);
    tree->insert(poly12);
    tree->insert(poly14);
    tree->insert(poly15);
    tree->insert(poly16);
    tree->insert(poly17);
    tree->insert(poly18);

    tree->print();
    tree->paint();
    */



    //SEGUNDO CASO

    /*
     RTree<string> *tree = new RTree<string>(2,5);
    
    Polygon<string> *poly = new Polygon<string>(535,324,"A");
    Polygon<string> *poly1 = new Polygon<string>(89,524,"B");
    Polygon<string> *poly2 = new Polygon<string>(1108,283,"C");
    Polygon<string> *poly3 = new Polygon<string>(1003,96,"D");

    Polygon<string> *poly4 = new Polygon<string>(288,89,"E");
    Polygon<string> *poly5 = new Polygon<string>(364,786,"F");
    Polygon<string> *poly6 = new Polygon<string>(1172,575,"G");
    Polygon<string> *poly7 = new Polygon<string>(1377,166,"H");

    Polygon<string> *poly8 = new Polygon<string>(1172,63,"I");
    Polygon<string> *poly9 = new Polygon<string>(661,504,"J");
    Polygon<string> *poly10 = new Polygon<string>(260,323,"K");
    Polygon<string> *poly11 = new Polygon<string>(69,216,"L");

    Polygon<string> *poly12 = new Polygon<string>(452,633,"M");
    //Polygon<string> *poly13 = new Polygon<string>(800,183,"N");

    tree->insert(poly);
    tree->insert(poly1);
    tree->insert(poly2);
    tree->insert(poly3);
    tree->insert(poly4);
    tree->insert(poly5);
    tree->insert(poly6);
    tree->insert(poly7);
    tree->insert(poly8);
    tree->insert(poly9);
    tree->insert(poly10);
    tree->insert(poly11);
    tree->insert(poly12);
    //tree->insert(poly13);

    tree->print();
    tree->paint();
    
    */
    
    //Tercer caso
    RTree<string> *tree = new RTree<string>(2,5);


    
    
    Polygon<string> *poly = new Polygon<string>(33,41,"A");
    Polygon<string> *poly1 = new Polygon<string>(36,230,"B");
    Polygon<string> *poly2 = new Polygon<string>(372,43,"C");
    Polygon<string> *poly3 = new Polygon<string>(363,222,"D");

    Polygon<string> *poly4 = new Polygon<string>(312,93,"E");
    Polygon<string> *poly5 = new Polygon<string>(89,152,"F");
    Polygon<string> *poly6 = new Polygon<string>(344,158,"G");
    Polygon<string> *poly7 = new Polygon<string>(78,66,"H");

    Polygon<string> *poly8 = new Polygon<string>(55,185,"I");
    Polygon<string> *poly9 = new Polygon<string>(357,99,"J");
    Polygon<string> *poly10 = new Polygon<string>(306,173,"K");
    Polygon<string> *poly11 = new Polygon<string>(323,136,"L");

    Polygon<string> *poly12 = new Polygon<string>(292,61,"M");
    Polygon<string> *poly13 = new Polygon<string>(298,122,"N");
    Polygon<string> *poly14 = new Polygon<string>(286,193,"O");
    Polygon<string> *poly15 = new Polygon<string>(328,75,"P");

    Polygon<string> *poly16 = new Polygon<string>(316,55,"Q");
    Polygon<string> *poly17 = new Polygon<string>(273,98,"R");
    Polygon<string> *poly18 = new Polygon<string>(325,185,"S");
    Polygon<string> *poly19 = new Polygon<string>(361,127,"T");

    Polygon<string> *poly20 = new Polygon<string>(284,144,"U");
    Polygon<string> *poly21 = new Polygon<string>(57,65,"V");
    Polygon<string> *poly22 = new Polygon<string>(64,209,"X");


    Polygon<string> *poly23 = new Polygon<string>(452,633,"Y");
    Polygon<string> *poly24 = new Polygon<string>(800,183,"Z");

    tree->insert(poly);
    tree->insert(poly1);
    tree->insert(poly2);
    tree->insert(poly3);
    tree->insert(poly4);
    tree->insert(poly5);
    tree->insert(poly6);
    tree->insert(poly7);
    tree->insert(poly8);
    tree->insert(poly9);
    tree->insert(poly10);
    tree->insert(poly11);
    tree->insert(poly12);
    tree->insert(poly13);
    tree->insert(poly14);
    tree->insert(poly15);
    tree->insert(poly16);
    tree->insert(poly17);
    tree->insert(poly18);
    tree->insert(poly19);
    tree->insert(poly20);
    tree->insert(poly21);
    tree->insert(poly22);
    tree->insert(poly23);
    tree->insert(poly24);
    
    Polygon<string> *poly25 = new Polygon<string>(470,650,"A1");
    Polygon<string> *poly26 = new Polygon<string>(500,660,"B1");
    Polygon<string> *poly27 = new Polygon<string>(400,60,"C1");
    Polygon<string> *poly28 = new Polygon<string>(250,630,"D1");
    
    tree->insert(poly25);
    tree->insert(poly26);
    tree->insert(poly27);
    tree->insert(poly28);
    
    Polygon<string> *poly29 = new Polygon<string>(390,109,"J1");
    Polygon<string> *poly30 = new Polygon<string>(370,90,"J2");
    Polygon<string> *poly31 = new Polygon<string>(374,99,"J3");
    Polygon<string> *poly32 = new Polygon<string>(400,99,"J4");
    
    tree->insert(poly29);
    tree->insert(poly30);
    tree->insert(poly31);
    tree->insert(poly32);
    
    
    Polygon<string> *poly33 = new Polygon<string>(420,119,"J5");
    Polygon<string> *poly34 = new Polygon<string>(410,99,"J6");
    Polygon<string> *poly35 = new Polygon<string>(430,200,"J7");
    Polygon<string> *poly36 = new Polygon<string>(450,150,"J8");
    
    tree->insert(poly33);
    tree->insert(poly34);
    tree->insert(poly35);
    tree->insert(poly36);
    
    
    Polygon<string> *poly37 = new Polygon<string>(520,119,"J9");
    Polygon<string> *poly38 = new Polygon<string>(510,99,"F0");
    Polygon<string> *poly39 = new Polygon<string>(530,200,"J11");
    Polygon<string> *poly40 = new Polygon<string>(550,150,"J12");
    
    tree->insert(poly37);
    tree->insert(poly38);
    tree->insert(poly39);
    tree->insert(poly40);
    
    tree->print();
    tree->paint();
    
    
    
    
   
    
    
    return 0;

}

