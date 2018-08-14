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

/*
 * Clase que representa el MBB para cada nodo
 */
class MBB
{
    private:
        float minX;
        float maxX;
        float minY;
        float maxY;
        int identifier;

    public:
        MBB(){}
        MBB(float minX, float maxX, float minY, float maxY, int identifier)
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

/*
 * Class Polygon that allows to create points and polygons
 */
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

/*
 * Class Node
 */

template <class T>
class Nodee
{
    private:
        int count; 
        int maxEntries;
        MBB<T> *mbb;
        Nodee<T> **children;
        Nodee<T> *father;
        Polygon<T> **polygons;
        bool leaf;
        bool intermediate;

    public:
        Nodee(){}
        Nodee(int maxEntries, int identifier)
        {
            this->count = 0;
            this->maxEntries = maxEntries;
            this->mbb = new MBB<T>(1000000,-1000000,1000000,-1000000, identifier);
            this->children = new Nodee<T>*[maxEntries];
            this->polygons = new Polygon<T>*[maxEntries];
            this->leaf = true;
            this->intermediate = false;
        }
        
    float getArea()
    {
        return ((mbb->maxX - mbb->minX)*(mbb->maxY - mbb->minY));
    }
    
    /*
     * Function to calculate a simulated area in order to view how much is growing a region considering the new polygon
     */
    
    float getSimulatedArea(Polygon<T> * polygon)
    {
        float minX = mbb->minX;
        float maxX = mbb->maxX;
        float minY = mbb->minY;
        float maxY = mbb->maxY;
        
        if(polygon->x < mbb->minX)
            mbb->minX = polygon->x;

        if(polygon->x > mbb->maxX)
            mbb->maxX = polygon->x;

        if(polygon->y < mbb->minY)
            mbb->minY = polygon->y;

        if(polygon->y > mbb->maxY)
            mbb->maxY = polygon->y;
        
        float area = getArea();
        
        mbb->minX = minX;
        mbb->maxX = maxX;
        mbb->minY = minY;
        mbb->maxY = maxY;
        
        return area;
    }
    
    float getSimulatedArea(Nodee<T> * node)
    {
        float minX = mbb->minX;
        float maxX = mbb->maxX;
        float minY = mbb->minY;
        float maxY = mbb->maxY;
        
        if(node->mbb->minX < mbb->minX)
            mbb->minX = node->mbb->minX;

        if(node->mbb->maxX > mbb->maxX)
            mbb->maxX = node->mbb->maxX;

        if(node->mbb->minY < mbb->minY)
            mbb->minY = node->mbb->minY;

        if(node->mbb->maxY > mbb->maxY)
            mbb->maxY = node->mbb->maxY;
        
        float area = getArea();
        
        mbb->minX = minX;
        mbb->maxX = maxX;
        mbb->minY = minY;
        mbb->maxY = maxY;
        
        return area;
    }
    
    
    
    friend class RTree<T>;
};

/*
 * Main class R-Tree
 */
template <class T>
class RTree
{
    private:
        Nodee<T> *root;
        int minEntries;
        int maxEntries;
        bool firstTime;
        int identifierCounter; // Counter for each node that allows in order to assign its identifier.
    
    public:
        RTree(){}
        RTree(int minEntries, int maxEntries)
        {
            this->identifierCounter = 0;
            this->root = new Nodee<T>(maxEntries, identifierCounter);
            identifierCounter++;
            this->minEntries = minEntries;
            this->maxEntries = maxEntries;
            this->firstTime = true;
        }

    
    /*
     * Function to update the dimensions of MBB
     */    
    void updateMBB(Nodee<T> *node, Polygon<T> *polygon)
    {
        do
        {
            if(polygon->x < node->mbb->minX)
                node->mbb->minX = polygon->x;

            if(polygon->x > node->mbb->maxX)
                node->mbb->maxX = polygon->x;

            if(polygon->y < node->mbb->minY)
                node->mbb->minY = polygon->y;

            if(polygon->y > node->mbb->maxY)
                node->mbb->maxY = polygon->y;
            
            node = node->father;
        }
        while(node != NULL);
        
    }
    
      void updateMBB(Nodee<T> *nodeF, Nodee<T> *nodeC)
    {
        do
        {
            
            if(nodeC->mbb->minX < nodeF->mbb->minX)
                nodeF->mbb->minX = nodeC->mbb->minX;

            if(nodeC->mbb->maxX > nodeF->mbb->maxX)
                nodeF->mbb->maxX = nodeC->mbb->maxX;

            if(nodeC->mbb->minY < nodeF->mbb->minY)
                nodeF->mbb->minY = nodeC->mbb->minY;

            if(nodeC->mbb->maxY > nodeF->mbb->maxY)
                nodeF->mbb->maxY = nodeC->mbb->maxY;
            
            nodeF = nodeF->father;
        }
        while(nodeF != NULL);
        
    }
    
    /*
     * Return the appropiate node to insert the polygon having in account how much the area is growing.
     */
    
    Nodee<T>* appropiateLeaf(Nodee<T>* node, Polygon<T> *polygon)
    {
        if(node->leaf)
            return node;
        
        float auxArea;
        float minArea = 1000000;
        Nodee<T> *auxNode;
        
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
     * Function that search the appropiate leaf to insert a new polygon
    */
    Nodee<T>* search(Polygon<T> *polygon)
    {
        return(appropiateLeaf(root, polygon));
    }
    
    /*
     * Function that find the two polygons with the longest distance.
     */
    void getExtremesPolygons(Nodee<T> *node, Polygon <T> *&firstExtremePolygon, Polygon <T> *&secondExtremePolygon)
    {
        float maxDistance = -1000000;
        float distance;
        
        float x1, x2;
        float y1, y2;
        
        for(int i=0; i<node->count; i++)
        {
            x1 = node->polygons[i]->x;
            y1 = node->polygons[i]->y;
            
            for(int j=i+1; j< node->count; j++)
            {
                x2 = node->polygons[j]->x;
                y2 = node->polygons[j]->y;
                
                distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));
                
                if(distance > maxDistance)
                {
                    maxDistance = distance;
                    firstExtremePolygon = node->polygons[i];
                    secondExtremePolygon = node->polygons[j];
                }
            }
        }
        
        firstExtremePolygon->intermediate = true;
        secondExtremePolygon->intermediate = true;
    }
    
    /*
     * This function fill each extreme node in firstPartNode and secondPartNode respectively.
     */
    void fillExtremeNodes(Nodee<T> *node, Nodee<T> *firstPartNode, Nodee<T> *secondPartNode)
    {
        Polygon<T> *firstExtremePolygon;
        Polygon<T> *secondExtremePolygon;        
        getExtremesPolygons(node, firstExtremePolygon, secondExtremePolygon);
 
        firstPartNode->polygons[firstPartNode->count] = firstExtremePolygon;
        firstPartNode->count++;
        updateMBB(firstPartNode, firstExtremePolygon);
        
        secondPartNode->polygons[secondPartNode->count] = secondExtremePolygon;
        secondPartNode->count++;
        updateMBB(secondPartNode, secondExtremePolygon);
  
    }
    
    
    float getDistance( Polygon<T> *p1, Polygon<T> *p2 )
    {
        return sqrt( pow(p1->x-p2->x,2) + pow(p1->y-p2->y,2));
    }
     
    
    void addNewPolygonNearToNode(Nodee<T> *node, Polygon<T> *polygon, Nodee<T> *firstPartNode, Nodee<T> *secondPartNode)
    {
        float distance = 0, area1, area2;
        float minDistance = 1000000;
        Polygon<T> *nearPolygon;
        bool firstNode = true;
        
        for(int i=0; i < node->count; i++)
        {
          
            if(node->polygons[i]->intermediate != true)
            {
                
                distance = getDistance(firstPartNode->polygons[0], node->polygons[i] ); 
                    
                if(distance == minDistance)
                {
                    area1 = firstPartNode->getSimulatedArea(nearPolygon) - firstPartNode->getArea();
                    area2 = firstPartNode->getSimulatedArea(node->polygons[i]) - firstPartNode->getArea();;
                    if(area2 < area1)
                    {
                        nearPolygon = node->polygons[i];
                        firstNode = true;
                    }

                }
                else if(distance < minDistance)
                {
                    minDistance = distance;
                    nearPolygon = node->polygons[i];                        
                    firstNode = true;
                }
                    
                
                distance = getDistance(secondPartNode->polygons[0], node->polygons[i]); 

                if(distance == minDistance)
                {
                    area1 = secondPartNode->getSimulatedArea(nearPolygon) - secondPartNode-> getArea();
                    area2 = secondPartNode->getSimulatedArea(node->polygons[i]) - secondPartNode->getArea();
                    
                    if(area2 < area1)
                    {
                        nearPolygon = node->polygons[i];
                        firstNode = false;
                    }
                }
                
                else if(distance < minDistance)
                {
                    minDistance = distance;
                    nearPolygon = node->polygons[i];
                    firstNode = false;
                }
                
            }
        }
        
        if(polygon->intermediate != true)
        {
            distance = getDistance(firstPartNode->polygons[0], polygon ); 
            
            if(distance == minDistance)
            {
                area1 = firstPartNode->getSimulatedArea(nearPolygon) - firstPartNode->getArea();
                area2 = firstPartNode->getSimulatedArea(polygon) - firstPartNode->getArea();
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
            
            distance = getDistance(secondPartNode->polygons[0], polygon );
            
            if(distance == minDistance)
            {
                area1 = secondPartNode->getSimulatedArea(nearPolygon) - secondPartNode->getArea();
                area2 = secondPartNode->getSimulatedArea(polygon) - secondPartNode->getArea();
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
            if((firstPartNode->count) < maxEntries-minEntries+1)
            {

                firstPartNode->polygons[firstPartNode->count] = nearPolygon;
                updateMBB(firstPartNode, nearPolygon);
                firstPartNode->count++;
            }
            else
            {
                secondPartNode->polygons[secondPartNode->count] = nearPolygon;
                updateMBB(secondPartNode, nearPolygon);
                secondPartNode->count++;
            }
            
        }
        
        else
        {
            if((secondPartNode->count) < maxEntries-minEntries+1)
            {
                secondPartNode->polygons[secondPartNode->count] = nearPolygon;
                updateMBB(secondPartNode, nearPolygon);
                secondPartNode->count++;
            }
            
            else
            {
                firstPartNode->polygons[firstPartNode->count] = nearPolygon;
                updateMBB(firstPartNode, nearPolygon);
                firstPartNode->count++;
            }
        }
        
        nearPolygon->intermediate = true;
        /*cout<<"======"<<endl;
        cout<<nearPolygon->identifier<<endl;*/
        
    }
    
     void getExtremesNodes(Nodee<T> *node, Nodee <T> *&firstExtremeNode, Nodee <T> *&secondExtremeNode)
    {
        float maxDistance = -1000000;
        float distance;
        
        float x1, x2;
        float y1, y2;
        
        for(int i=0; i<node->count; i++)
        {
            x1 = (node->children[i]->mbb->maxX + node->children[i]->mbb->minX )/2;
            y1 = (node->children[i]->mbb->maxY + node->children[i]->mbb->minY )/2;
            
            for(int j=i+1; j< node->count; j++)
            {
                x2 = (node->children[j]->mbb->maxX + node->children[j]->mbb->minX )/2;
                y2 = (node->children[j]->mbb->maxY + node->children[j]->mbb->minY )/2;
                
                distance = sqrt(pow((x1 - x2),2) +pow((y1 - y2),2));
                
                if(distance > maxDistance)
                {
                    maxDistance = distance;
                    firstExtremeNode = node->children[i];
                    secondExtremeNode = node->children[j];                    
                }
            }
        }
        
        firstExtremeNode->intermediate = true;
        secondExtremeNode->intermediate = true;
    }
    
    void addNewNodeNearToNode(Nodee<T> *node, Nodee<T> *newBrotherNode, Nodee<T> *firstPartNode, Nodee<T> *secondPartNode)
    {
        float area;
        float minArea = 1000000;
        Nodee<T> *nearNode;
        bool firstNode = true;
        
        for(int i=0; i < node->count; i++)
        {
          
            if(node->children[i]->intermediate != true)
            {
                
                area = firstPartNode->getSimulatedArea(node->children[i]) - firstPartNode->getArea(); 

                if(area < minArea)
                {
                    minArea = area;
                    nearNode = (node->children[i]);                        
                    firstNode = true;
                }
               
            
                area = secondPartNode->getSimulatedArea((node->children[i])) - secondPartNode->getArea(); 

                if(area < minArea)
                {
                    minArea = area;
                    nearNode = (node->children[i]);
                    firstNode = false;
                }

            }
        }
        
        if(newBrotherNode->intermediate != true)
        {
            area = firstPartNode->getSimulatedArea(newBrotherNode) - firstPartNode->getArea(); 
            if(area < minArea)
            {
                minArea = area;
                nearNode = newBrotherNode;
                firstNode = true;
            }
            
            area = secondPartNode->getSimulatedArea(newBrotherNode) - secondPartNode->getArea(); 
            if(area < minArea)
            {
                minArea = area;
                nearNode = newBrotherNode;
                firstNode = false;
            }
        }
        
        if(firstNode)
        {
            if((firstPartNode->count) < maxEntries-minEntries+1)
            {
                nearNode->father = firstPartNode;
                firstPartNode->children[firstPartNode->count] = nearNode;
                updateMBB(firstPartNode, nearNode);
                firstPartNode->count++;
            }
            else
            {
                nearNode->father = secondPartNode;
                secondPartNode->children[secondPartNode->count] = nearNode;
                updateMBB(secondPartNode, nearNode);
                secondPartNode->count++;
            }
            
        }
        
        else
        {
            if((secondPartNode->count) < maxEntries-minEntries+1)
            {
                nearNode->father = secondPartNode;
                secondPartNode->children[secondPartNode->count] = nearNode;
                updateMBB(secondPartNode, nearNode);
                secondPartNode->count++;
            }
            
            else
            {
                nearNode->father = firstPartNode;
                firstPartNode->children[firstPartNode->count] = nearNode;
                updateMBB(firstPartNode, nearNode);
                firstPartNode->count++;
            }
        }
        
        nearNode->intermediate = true;
        //cout<<"near"<<nearNode->mbb->identifier<<endl;
    }
    
    
    
    void addPolygonsToNode(Nodee<T> *node, Polygon<T> *polygon, Nodee<T> *firstPartNode, Nodee<T> *secondPartNode)
    {
        int remainedNodes = maxEntries - 2 + 1;
        
                
        while(remainedNodes>0)
        {
            
            addNewPolygonNearToNode(node, polygon, firstPartNode, secondPartNode);
            
            remainedNodes--;
        }
        
        for(int i = 0; i<firstPartNode->count; i++)
        {
            firstPartNode->polygons[i]->intermediate = false;
        }
        
        for(int i = 0; i<secondPartNode->count; i++)
        {
            secondPartNode->polygons[i]->intermediate = false;
        }
    }
    
    void addNodesToNodeInter(Nodee<T> *node, Nodee<T> *newBrotherNode, Nodee<T> *firstPartNode, Nodee<T> *secondPartNode)
    {
        int remainedNodes = maxEntries - 2 + 1;
        
        while(remainedNodes>0)
        {
            addNewNodeNearToNode(node, newBrotherNode, firstPartNode, secondPartNode);          
            remainedNodes--;
        }
        
        for(int i=0; i<firstPartNode->count; i++)
            firstPartNode->children[i]->intermediate = false;
        
        for(int i=0; i<secondPartNode->count; i++)
            secondPartNode->children[i]->intermediate = false;
        
    }
    
    
    void fillExtremeNodesInter(Nodee<T> *node, Nodee<T> *firstPartNode, Nodee<T> *secondPartNode)
    {
        Nodee<T> *firstExtremeNode;
        Nodee<T> *secondExtremeNode;        
        getExtremesNodes(node, firstExtremeNode, secondExtremeNode);
     
        firstExtremeNode->father = firstPartNode;
        firstPartNode->children[firstPartNode->count] = firstExtremeNode;//firstExtremeNode->copy()
        firstPartNode->count++;
        updateMBB(firstPartNode, firstExtremeNode);
        
        secondExtremeNode->father = secondPartNode;
        secondPartNode->children[secondPartNode->count] = secondExtremeNode;//*secondExtremePolygon;
        secondPartNode->count++;
        updateMBB(secondPartNode, secondExtremeNode);
    }
    
    
    
    void insertNode(Nodee<T> *node, Nodee<T> *newBrotherNode)
    {
        Nodee<T> *nf = node->father;
        if(nf == NULL)//significa que llegue al root
        {
            Nodee<T> *newRoot = new Nodee<T>(maxEntries, 0);
            newRoot->leaf = false;
            
            node->father = newRoot;
            newRoot->children[newRoot->count]= node;
            updateMBB(newRoot, node);
            node->mbb->identifier = identifierCounter;
            identifierCounter++;
            newRoot->count++;
            
            newBrotherNode->father = newRoot;
            newRoot->children[newRoot->count]= newBrotherNode;
            updateMBB(newRoot, newBrotherNode);
            newRoot->count++;
            
            
            root = newRoot; 
        }
        
        
        else
        {     
            if( (nf->count) < maxEntries)
            {
               
                newBrotherNode->father = nf;
                nf->children[nf->count] = newBrotherNode;
                updateMBB(nf, newBrotherNode);
                nf->count++;
                
                
                
            }
            else
            {
                Nodee<T> *firstPartNode;
                Nodee<T> *secondPartNode;
                
                               
                firstPartNode = new Nodee<T>(maxEntries, nf->mbb->identifier);
                firstPartNode->leaf = false;
                secondPartNode = new Nodee<T>(maxEntries, identifierCounter);
                secondPartNode->leaf = false;
                identifierCounter++;
                                
                firstPartNode->father = nf->father;
               
                
                fillExtremeNodesInter(nf, firstPartNode, secondPartNode);
                addNodesToNodeInter(nf, newBrotherNode, firstPartNode, secondPartNode);                        
                
                
                /*cout<<"Ini"<<endl;
                print(firstPartNode,0);
                cout<<"---"<<endl;
                print(secondPartNode,0);
                cout<<"Fin"<<endl;*/
               
                
                
                *nf = *firstPartNode;
                nf = firstPartNode;
                
                
                
                insertNode(nf, secondPartNode);
                //cout<<"tam"<<node->father->count<<endl;
            }
        }
    }
    
    
    
     
    /*
     *  Function to insert a polygon
     */ 
    void insert(Polygon<T> *polygon)
    {

        if(firstTime)
        {
            root->polygons[root->count] = polygon;
            root->count++;
            firstTime = false;
            updateMBB(root, polygon);
            return;
        }    
        
     
        Nodee<T> *node = search(polygon);//Nodo más cercano   
        //cout<<"Nodo apropiado más cercano:"<<node->mbb->identifier<<endl;

        if( (node->count) < maxEntries)
        {
            node->polygons[node->count] = polygon;
            node->count++;
            updateMBB(node, polygon);
        }
        else
        {
            
            Nodee<T> *firstPartNode;
            Nodee<T> *secondPartNode;
      
            
            firstPartNode = new Nodee<T>(maxEntries, node->mbb->identifier);
            secondPartNode = new Nodee<T>(maxEntries, identifierCounter);
            identifierCounter++;
            
            firstPartNode->father = node->father;
            
            fillExtremeNodes(node, firstPartNode, secondPartNode);   
          
            addPolygonsToNode(node, polygon, firstPartNode, secondPartNode);   
            
            /*cout<<"Ini"<<endl;
            print(firstPartNode,0);
            cout<<"--"<<endl;
            print(secondPartNode,0);
            cout<<"Fin"<<endl; */
            
            *node = *firstPartNode;
            node =  firstPartNode;
            
            
            
            /*if(node->father != NULL)
            {
                print(node->father,0);
                cout<<"-------"<<endl;
                cout<<"Fin"<<endl;
            }*/
            
            insertNode(node, secondPartNode);
            /*if(node->father!=NULL)
            {
                cout<<"Aqui Aqui Aqui: "<< node->father->leaf<< " Cout: "<<node->father->count<<endl;
                print(node->father, 0);
            }*/
            //cout<<"Aqui: "<< nf->leaf<< " Cout: "<<nf->count<<endl;
            //print(nf, 0);
        }
        
    }
        
    
    void print(Nodee<T> *node, int nivel)
    {
        if(node == NULL)
            cout<<"Null Node"<<endl;
        
        string tabs= "";
        for(int i=0; i<nivel; i++)
            tabs = tabs + "\t";
        cout<<tabs<<"Rectangle: R"<<node->mbb->identifier;
        cout<<" x:"<<node->mbb->minX<<"-"<<node->mbb->maxX;
        cout<<" y:"<<node->mbb->minY<<"-"<<node->mbb->maxY;
        
        cout<< "Cant: "<<node->count<<endl;
        
        if(node->father != NULL)
            cout<< "============== "<<node->father->mbb->identifier<<endl;    
        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                if(node->father != NULL)
                    cout<<tabs<<"Poly: "<<node->polygons[i]->identifier<<" Father: R"<< node->father->mbb->identifier <<endl;
                else
                    cout<<tabs<<"Poly: "<<node->polygons[i]->identifier <<endl;
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
    
    /*
         * Function print:
         *  It allows to print the node's identifiers and the polygons' identifiers.
         *  It starts from the root to the leafs.
    */
    
    void print()
    {
        print(root, 0);
    }
    


    vector <Scalar> colores;
    void paint(Nodee<T> *node, int nivel, Mat &img)
    {
        int scale = 1;
        float x1 = node->mbb->minX*scale;
        float x2 = node->mbb->maxX*scale;
        float y1 = 700-node->mbb->minY*scale;
        float y2 = 700-node->mbb->maxY*scale;
        rectangle(img, Point2f(x1, y1), Point2f(x2, y2), colores[nivel], 6-nivel, 8, 0);
        putText(img, "R"+to_string(node->mbb->identifier) , Point2f(x1+6,y2-6), FONT_ITALIC, 1-0.1*nivel,  colores[nivel], 2 , 8 , false);

        
        if(node->leaf)
        {
            for(int i=0; i<node->count; i++)
            {
                x1 = node->polygons[i]->x*scale;
                y1 = 700-node->polygons[i]->y*scale;
                circle(img, Point2f(x1, y1), 6, Scalar(255, 255, 255), -1, 8, 0);
                putText(img, node->polygons[i]->identifier , Point2f(x1-15,y1+20), FONT_ITALIC, 0.5,  Scalar(255, 255, 255), 2 , 8 , false);
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
        Mat img(900, 1400, CV_8UC3, Scalar(0,0,0));
        paint(root, 0, img);
        imshow("R0", img);        
        waitKey(0);
    }
};


int main(int argc, char const *argv[])
{    
    
    //CASO 0
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
    
    Polygon<string> *poly14 = new Polygon<string>(13,8,"0");
    tree->insert(poly14);
    Polygon<string> *poly15 = new Polygon<string>(14,9,"P");
    tree->insert(poly15);
    Polygon<string> *poly16 = new Polygon<string>(15,8,"Q");
    tree->insert(poly16);
    
    Polygon<string> *poly17 = new Polygon<string>(15,11,"R");
    tree->insert(poly17);
    
    Polygon<string> *poly18 = new Polygon<string>(4,8,"S");
    tree->insert(poly18);
    tree->print();
    tree->paint();*/
    
    //PRIMER CASO
    
    /*RTree<string> *tree = new RTree<string>(2,5);
    
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
    tree->paint();*/
    

    
    
    
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
    Polygon<string> *poly13 = new Polygon<string>(800,183,"N");
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
    
    Polygon<string> *poly25 = new Polygon<string>(470,650,"A1");
    Polygon<string> *poly26 = new Polygon<string>(500,660,"B1");
    Polygon<string> *poly27 = new Polygon<string>(400,60,"C1");
    Polygon<string> *poly28 = new Polygon<string>(250,630,"D1");    
    Polygon<string> *poly29 = new Polygon<string>(390,109,"E1");
    
    Polygon<string> *poly30 = new Polygon<string>(370,90,"F1");
    Polygon<string> *poly31 = new Polygon<string>(374,99,"G1");
    Polygon<string> *poly32 = new Polygon<string>(400,99,"H1");
    Polygon<string> *poly33 = new Polygon<string>(420,119,"I1");
    Polygon<string> *poly34 = new Polygon<string>(410,99,"J1");
    
    Polygon<string> *poly35 = new Polygon<string>(430,200,"K1");
    Polygon<string> *poly36 = new Polygon<string>(450,150,"L1");
    Polygon<string> *poly37 = new Polygon<string>(520,119,"M1");
    Polygon<string> *poly38 = new Polygon<string>(510,99,"N1");
    Polygon<string> *poly39 = new Polygon<string>(530,200,"01");
    
    Polygon<string> *poly40 = new Polygon<string>(550,150,"P1");
    
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
    
    
    
    tree->insert(poly25);
    tree->insert(poly26);
    tree->insert(poly27);
    tree->insert(poly28);    
    tree->insert(poly29);
    
    tree->insert(poly30);
    tree->insert(poly31);
    tree->insert(poly32);
    tree->insert(poly33);
    tree->insert(poly34);
    
    tree->insert(poly35);
    tree->insert(poly36);    
    tree->insert(poly37);
    tree->insert(poly38);
    tree->insert(poly39);
    
    tree->insert(poly40);
    
    tree->insert(new Polygon<string> (414,214 ,"A2"));
    tree->insert(new Polygon<string> (389,381 ,"B2"));
    tree->insert(new Polygon<string> (810,516 ,"C2"));
    tree->insert(new Polygon<string> (819,188 ,"D2"));
    tree->insert(new Polygon<string> (545,380 ,"E2"));
    tree->insert(new Polygon<string> (284,410 ,"F2"));
    tree->insert(new Polygon<string> (620,634 ,"G2"));
    tree->insert(new Polygon<string> (934,428 ,"H2"));
    tree->insert(new Polygon<string> (480,446 ,"I2"));
    tree->insert(new Polygon<string> (420,446 ,"J2"));
    
    tree->insert(new Polygon<string> (413,217 ,"A21"));
    tree->insert(new Polygon<string> (381,381 ,"B21"));
    tree->insert(new Polygon<string> (817,514 ,"C21"));
    tree->insert(new Polygon<string> (810,188 ,"D21"));
    tree->insert(new Polygon<string> (542,380 ,"E21"));
    tree->insert(new Polygon<string> (283,419 ,"F21"));
    tree->insert(new Polygon<string> (621,634 ,"G21"));
    tree->insert(new Polygon<string> (932,427 ,"H21"));
    tree->insert(new Polygon<string> (483,443 ,"I21"));
    tree->insert(new Polygon<string> (425,440 ,"J21"));
    
    
    Polygon<string> *poly41 = new Polygon<string>(535,324,"A3");
    Polygon<string> *poly42 = new Polygon<string>(89,524,"B3");
    Polygon<string> *poly43 = new Polygon<string>(118,283,"C3");
    Polygon<string> *poly44 = new Polygon<string>(703,96,"D3");
    Polygon<string> *poly45 = new Polygon<string>(288,89,"E3");
    Polygon<string> *poly46 = new Polygon<string>(364,386,"F3");
    Polygon<string> *poly47 = new Polygon<string>(672,575,"G3");
    Polygon<string> *poly48 = new Polygon<string>(777,166,"H3");
    Polygon<string> *poly49 = new Polygon<string>(572,63,"I3");
    Polygon<string> *poly50 = new Polygon<string>(661,504,"J3");
    Polygon<string> *poly51 = new Polygon<string>(260,323,"K3");
    Polygon<string> *poly52 = new Polygon<string>(69,216,"L3");
    Polygon<string> *poly53 = new Polygon<string>(452,633,"M3");
    Polygon<string> *poly54 = new Polygon<string>(800,183,"N3");

    tree->insert(poly41);
    tree->insert(poly42);
    tree->insert(poly43);
    tree->insert(poly44);
    tree->insert(poly45);
    tree->insert(poly46);
    tree->insert(poly47);
    tree->insert(poly48);
    tree->insert(poly49);
    tree->insert(poly50);
    tree->insert(poly51);
    tree->insert(poly52);
    tree->insert(poly53);
    tree->insert(poly54);
    
    Polygon<string> *poly55 = new Polygon<string>(525,324,"A31");
    Polygon<string> *poly56 = new Polygon<string>(86,524,"B31");
    Polygon<string> *poly57 = new Polygon<string>(138,283,"C31");
    Polygon<string> *poly58 = new Polygon<string>(713,96,"D31");
    Polygon<string> *poly59 = new Polygon<string>(258,89,"E31");
    Polygon<string> *poly60 = new Polygon<string>(394,386,"F31");
    Polygon<string> *poly61 = new Polygon<string>(622,575,"G31");
    Polygon<string> *poly62 = new Polygon<string>(717,166,"H31");
    Polygon<string> *poly63 = new Polygon<string>(502,63,"I31");
    Polygon<string> *poly64 = new Polygon<string>(641,504,"J31");
    Polygon<string> *poly65 = new Polygon<string>(220,323,"K31");
    Polygon<string> *poly66 = new Polygon<string>(60,216,"L31");
    Polygon<string> *poly67 = new Polygon<string>(412,633,"M31");
    Polygon<string> *poly68 = new Polygon<string>(820,183,"N31");
    
    tree->insert(poly55);
    tree->insert(poly56);
    tree->insert(poly57);
    tree->insert(poly58);
    tree->insert(poly59);
    tree->insert(poly60);
    tree->insert(poly61);
    tree->insert(poly62);
    tree->insert(poly63);
    tree->insert(poly64);
    tree->insert(poly65);
    tree->insert(poly66);
    tree->insert(poly67);
    tree->insert(poly68);
    
    tree->insert(new Polygon<string> (213,317 ,"A4"));
    tree->insert(new Polygon<string> (481,481 ,"B4"));
    tree->insert(new Polygon<string> (117,514 ,"C4"));
    tree->insert(new Polygon<string> (810,688 ,"D4"));
    tree->insert(new Polygon<string> (342,280 ,"E4"));
    tree->insert(new Polygon<string> (583,119 ,"F4"));
    tree->insert(new Polygon<string> (721,234 ,"G4"));
    tree->insert(new Polygon<string> (932,327 ,"H4"));
    tree->insert(new Polygon<string> (283,443 ,"I4"));
    tree->insert(new Polygon<string> (125,540 ,"J4"));
    
    
    tree->insert(new Polygon<string> (213,347 ,"A5"));
    tree->insert(new Polygon<string> (451,671 ,"B5"));
    tree->insert(new Polygon<string> (677,654 ,"C5"));
    tree->insert(new Polygon<string> (360,298 ,"D5"));
    tree->insert(new Polygon<string> (352,340 ,"E5"));
    tree->insert(new Polygon<string> (123,569 ,"F5"));
    tree->insert(new Polygon<string> (821,674 ,"G5"));
    tree->insert(new Polygon<string> (442,327 ,"H5"));
    tree->insert(new Polygon<string> (673,423 ,"I5"));
    tree->insert(new Polygon<string> (395,210 ,"J5"));
    
    tree->insert(new Polygon<string> (512,517 ,"A6"));
    tree->insert(new Polygon<string> (283,371 ,"B6"));
    tree->insert(new Polygon<string> (418,214 ,"C6"));
    tree->insert(new Polygon<string> (715,488 ,"D6"));
    tree->insert(new Polygon<string> (740,680 ,"E6"));
    tree->insert(new Polygon<string> (382,319 ,"F6"));
    tree->insert(new Polygon<string> (625,534 ,"G6"));
    tree->insert(new Polygon<string> (738,627 ,"H6"));
    tree->insert(new Polygon<string> (887,243 ,"I6"));
    tree->insert(new Polygon<string> (125,540 ,"J6"));
    
     tree->insert(new Polygon<string> (423,117 ,"A7"));
    tree->insert(new Polygon<string> (432,71 ,"B7"));
    tree->insert(new Polygon<string> (418,74 ,"C7"));
    tree->insert(new Polygon<string> (415,108 ,"D7"));
    tree->insert(new Polygon<string> (440,210 ,"E7"));
    tree->insert(new Polygon<string> (442,119 ,"F7"));
    tree->insert(new Polygon<string> (425,214 ,"G7"));
    tree->insert(new Polygon<string> (438,127 ,"H7"));
    tree->insert(new Polygon<string> (427,63 ,"I7"));
    tree->insert(new Polygon<string> (425,70 ,"J7"));
    
    
    
    
    tree->print();
    tree->paint();
    
    //Cuarto Caso
    /*RTree<string> *tree = new RTree<string>(2,5);
    tree->insert(new Polygon<string> (414,214 ,"B"));
    tree->insert(new Polygon<string> (389,381 ,"C"));
    tree->insert(new Polygon<string> (810,516 ,"D"));
    tree->insert(new Polygon<string> (819,188 ,"E"));
    tree->insert(new Polygon<string> (545,380 ,"F"));
    tree->insert(new Polygon<string> (284,410 ,"G"));
    tree->insert(new Polygon<string> (620,634 ,"H"));
    tree->insert(new Polygon<string> (934,428 ,"I"));
    tree->insert(new Polygon<string> (480,446 ,"J"));
    tree->insert(new Polygon<string> (420,446 ,"K"));
    tree->print();
    tree->paint();*/
    
    
    return 0;

}
