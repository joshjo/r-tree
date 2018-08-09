#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include "Point.h"

#if !defined(RTREE_H)
#define RTREE_H

typedef int DataType;
typedef Point<DataType> P;

/*
 * Struct Interval
 * ...........................................................................
 * Define un intervalo en una dimensión. 
 * Utilizado para definir rectangulos.
 */
struct Interval{
    DataType min;
    DataType max;
    Interval(): min(0), max(0){};
    Interval(DataType _min,
             DataType _max): min(_min), max(_max){}

    bool inRange(DataType &val){return ( min<val && val<max );}  
    void join(Interval &Ik){
        min = (Ik.min<min) ? Ik.min : min;
        max = (Ik.max>min) ? Ik.max : max;
    }
    Interval getJoin(Interval &Ik){
        DataType newmin = (Ik.min<min) ? Ik.min : min;
        DataType newmax = (Ik.max>min) ? Ik.max : max;
        return Interval(newmin,newmax);
    }
    DataType length(){return max-min;}
    bool operator==(const Interval& other){
        return (this->min==other.min && this->max==other.max);
    }
};




/*
 * Class Rectangle
 * ...........................................................................
 * Solo disponible en dos dimensiones.
 * Utilizado para definir el MBR.
 */
class Rectangle
{
private:
    size_t _dimension;
    Interval *_Ik;

public:
    Rectangle():_dimension(0){}
    Rectangle(size_t dimension): _dimension(dimension){
        _Ik = new Interval [dimension];
    }
    Rectangle(DataType minX, DataType maxX,DataType minY, DataType maxY){
        _Ik = new Interval [2];
        _Ik[0] = Interval(minX,maxX);
        _Ik[1] = Interval(minY,maxY);
    }
    Rectangle(Interval x, Interval y){
        _Ik = new Interval [2];
        _Ik[0] = x;
        _Ik[1] = y;
    }

    void set(Interval *Ik){ _Ik = Ik;}
    bool inRange(DataType *val);
    bool inRange(Point<DataType> p);
    void join(Rectangle *rect);
    DataType area();

    Rectangle getJoin(Rectangle &rect);
    Rectangle getJoin(Rectangle *rect);
    size_t    getDimension(){return _dimension;}
    Interval* getIntervals(){return _Ik;}
    Interval* getIntervals(int dimension){return &_Ik[dimension];}

    std::vector<P> getEnds();
    void print(bool basic);

    bool operator==(const Rectangle& other){
        for(int i=0; i<this->_dimension; ++i){
            if( !(this->_Ik[i]==other._Ik[i])  ) return false;
            // Si produce error, usar getIntervals 
        }
        return true;
    }
};


/*
 * In range
 * 
 * Verifica que un punto, definido por un puntero, esta dentro del rectangulo
 * 
 */
bool Rectangle::inRange(DataType *val){
    if (_dimension == 2)
        return this->_Ik[0].inRange(val[0]) * this->_Ik[1].inRange(val[1]);
    else if (_dimension == 1)
        return this->_Ik[0].inRange(val[0]);
    else if (_dimension == 3)
        return this->_Ik[0].inRange(val[0]) * this->_Ik[1].inRange(val[1]) * this->_Ik[2].inRange(val[2]);
    else{
        bool r = true;
        for(int i = 0; i< _dimension; ++i) r *= this->_Ik[i].inRange(val[i]);
        return r;
    }
}  


/*
 * In range
 * 
 * Verifica que un punto esta dentro del rectangulo
 * 
 */
bool Rectangle::inRange(Point<DataType> p){
    return this->_Ik[0].inRange(p.x) * this->_Ik[1].inRange(p.y);
}


/*
 * Join
 * 
 * Juntar con otro rectangulo.
 * 
 */
void Rectangle::join(Rectangle *rect){
    if (_dimension == 2){
        Interval* ik = rect->getIntervals();
        this->_Ik[0].join( ik[0] );
        this->_Ik[1].join( ik[1] );
    }
    else if(_dimension == 0){
        _Ik        = rect->getIntervals();
        _dimension = rect->getDimension();
    }
    else{
        Interval* ik = rect->getIntervals();
        for(int i = 0; i<this->_dimension; ++i) this->_Ik[i].join( ik[i] );
    }
}


/*
 * Get Join
 * 
 * Obtener el resultado de unir con otro rectangulo
 * 
 */
Rectangle Rectangle::getJoin(Rectangle &rect){
    if (_dimension == 2){
        Interval* ik = rect.getIntervals();
        Interval x = this->_Ik[0].getJoin( ik[0] );
        Interval y = this->_Ik[1].getJoin( ik[1] );
        return Rectangle(x,y);
    }
    else if(_dimension == 0){
        return rect;
    }
    else{
        cout << "Rectangle::getJoin: Esto no esta listo :c " << endl;
        return Rectangle();
    }
}
Rectangle Rectangle::getJoin(Rectangle *rect){
    if (_dimension == 2){
        Interval* ik = rect->getIntervals();
        Interval x = this->_Ik[0].getJoin( ik[0] );
        Interval y = this->_Ik[1].getJoin( ik[1] );
        return Rectangle(x,y);
    }
    else if(_dimension == 0){
        return rect[0];
    }
    else{
        cout << "Rectangle::getJoin: Esto no esta listo :c " << endl;
        return Rectangle();
    }
}


/*
 * Area
 * 
 * Obtener el area del rectangulo
 * 
 */
DataType Rectangle::area(){
    if (_dimension == 2){
        return this->_Ik[0].length() * this->_Ik[1].length();
    }
    else if(_dimension == 0){
        return 0;
    }
    else{
        cout << "Rectangle::area: Esto no esta listo :c " << endl;
        return 0;
    }
}


/*
 * Get rectangle ends
 * 
 * Obtener extremos del rectangulo
 * Es decir: Point(minX, minY), Point(maxX, maxY)
 * Solo funciona para dos dimensiones
 * 
 */
std::vector<P> Rectangle::getEnds(){
    std::vector<P> ends;
    ends.push_back( P(_Ik[0].min,_Ik[1].min) );
    ends.push_back( P(_Ik[0].max,_Ik[1].max) );
    return ends;
}


/*
 * Simple Print
 * 
 */
void Rectangle::print(bool basic = false){
    if( !basic ) printf ("Rectangulo:\n");
    for(int i=0;i<_dimension;++i) printf ("[%i,%i]\t",this->_Ik[i].min,
                                                      this->_Ik[i].max);
    if( !basic ) printf ("\n\n");
}










/*
 * Class Object: Polygon and PointRG
 * ...........................................................................
 */
class Object
{
protected:
    Rectangle _MBR;
    std::string identifier;
public:
    Rectangle* getMBR(){return &this->_MBR;};
    void printMBR(){
        printf("Object %s:\n.......................\n",identifier.c_str());
        printf("MBR:\t");
        _MBR.print(true);
        printf("\n\n");
    }
    virtual void updateMBR() = 0;
    //virtual bool iswithin(Object    &d) = 0;
    virtual bool iswithin(Rectangle *d) = 0;
    virtual DataType howMuchGrow(Rectangle *r) = 0;
    virtual void* search(Object *dat) = 0;
};


/*
 * Class Polygon
 * ...........................................................................
 * Poligonos de dos dimensiones.
 * Formado por un vector de puntos.
 */
class Polygon: public Object
{
private:
    std::vector< Point<DataType> > _data;
    
public:
    Polygon();
    Polygon(std::vector< P > &data){ 
        _data = data;
        this->updateMBR();
    }

    void updateMBR();
    bool iswithin(Polygon   &d);
    bool iswithin(Rectangle *d);
    DataType howMuchGrow(Rectangle *r);
    void* search(Object *dat);

    std::vector<P> get_points();
};


/*
 * UpdateMBR
 * 
 * Actualizar MBR debido a un cambio en el vector de puntos.
 * 
 */
void Polygon::updateMBR(){
    DataType minX =  INF, 
             maxX = -INF, 
             minY =  INF, 
             maxY = -INF;

    for (int i=0; i<this->_data.size(); ++i){
        if( minX > this->_data[i].x ) minX = this->_data[i].x;
        if( maxX < this->_data[i].x ) maxX = this->_data[i].x;
        if( minY > this->_data[i].y ) minY = this->_data[i].y;
        if( maxY < this->_data[i].y ) maxY = this->_data[i].y;
    }
    this->_MBR = Rectangle( Interval(minX,maxX),Interval(minY,maxY) );
}


/*
 * Polygon is within Object
 * 
 * Retorna verdadero si el poligono está dentro del objeto. 
 * 
 */
bool Polygon::iswithin(Polygon &d){
    Rectangle *objMBR = d.getMBR();
    for (int i=0; i<this->_data.size(); ++i){
        if(  !objMBR->inRange( this->_data[i] )  ) return false;
    }
    return true;
}


/*
 * Polygon is within Rectangle
 * 
 * Retorna verdadero si el poligono está dentro del rectangulo. 
 * 
 */
bool Polygon::iswithin(Rectangle *d){
    for (int i=0; i<this->_data.size(); ++i){
        if(  d->inRange( this->_data[i] )  ) return false;
    }
    return true;
}


/*
 * How much does the rectangle grow?
 * 
 * Retorna el aumento de area del rectangulo si se añade el poligono.
 * 
 */
DataType Polygon::howMuchGrow(Rectangle *r){
    Rectangle newR = this->_MBR.getJoin(r[0]);
    return newR.area() - r->area();
}

/*
 * Fool Search
 * 
 */
void* Polygon::search(Object *dat){
    Rectangle * datMBR = dat->getMBR();
    if( datMBR[0] == this->_MBR ) this;
    else NULL;
}


/*
 * Get points (Get MBR ends)
 * 
 * Retorna los extremos del MBR
 * 
 */
std::vector<P> Polygon::get_points(){
    return this->_MBR.getEnds();
}








/*
 * Class Point RG
 * ...........................................................................
 */
class PointRG: public Object
{
private:
    Point<DataType> _data;

public:
    PointRG( Point<DataType> &data ){
        this->_data = data;
        this->updateMBR();  // <- No necesario?
    }
    void updateMBR();
    bool iswithin(Polygon  &d);
    bool iswithin(Rectangle *d);
    DataType howMuchGrow(Rectangle *r);
};


/*
 * UpdateMBR
 * 
 * Define MBR. Los cuatros puntos son iguales al dato
 * 
 */
void PointRG::updateMBR(){
    this->_MBR = Rectangle( Interval(_data.x,_data.x), 
                            Interval(_data.y,_data.y) );
}


/*
 * PointRG is within Object
 * 
 * Retorna verdadero si el punto está dentro del objeto. 
 * 
 */
bool PointRG::iswithin(Polygon &d){
    Rectangle *objMBR = d.getMBR();
    return objMBR->inRange( _data );
}


/*
 * PointRG is within Rectangle
 * 
 * Retorna verdadero si el punto está dentro del rectangulo. 
 * 
 */
bool PointRG::iswithin(Rectangle *d){
    return d->inRange( _data );
}


/*
 * How much does the rectangle grow?
 * 
 * Retorna el aumento de area del rectangulo si se añade el punto.
 * 
 */
DataType PointRG::howMuchGrow(Rectangle *r){
    Rectangle newR = this->_MBR.getJoin(r[0]);
    return newR.area() - r->area();
}








/*
 * Class Node
 * ...........................................................................
 */
class Node
{
private:
    bool _isleaf;   // Es hoja?
    bool _isroot;   // Es raiz?
    
    int    _minEntries;     // Minima cantidad de elementos en el nodo
    int    _maxEntries;     // Maxima cantidad de elementos en el nodo
    int    _dimension;      // Dimension de los datos
    size_t _size;           // Cantidad de elementos en el nodo

    Rectangle _MBR      ;
    Polygon * _childPoly;
    Node    * _childNode;

    void add(Polygon &element);
    void add(Node    &element);
    void add(Node    *element);
    void reset();

    void endsRectangles(Rectangle* rects,size_t &length, int &extreme1, int &extreme2);
    void endsRectangles(Polygon* poly,size_t &length, int &extreme1, int &extreme2);
    void endsRectangles(Node* nodo,size_t &length, int &extreme1, int &extreme2);

public:
/*
 *  Constructors
 */
    Node(): _minEntries(0), _maxEntries(0), _dimension(0), 
            _size(0), 
            _isleaf(true), _isroot(false){}
    Node(int minEntries, 
         int maxEntries, 
         int dimension): _minEntries(minEntries), _maxEntries(maxEntries), _dimension(dimension), 
                         _size(0),
                         _isleaf(true), _isroot(false){
        _childPoly = new Polygon[maxEntries + 1];
    }
    Node(int  minEntries, 
         int  maxEntries, 
         int  dimension ,
         bool isLeaf): _minEntries(minEntries), _maxEntries(maxEntries), _dimension(dimension), 
                         _size(0),
                         _isleaf(isLeaf), _isroot(false){
        if(_isleaf) _childPoly = new Polygon[_maxEntries + 1];
        else        _childNode = new Node   [_maxEntries + 1];
    }
    Node(int  minEntries, 
         int  maxEntries, 
         int  dimension ,
         bool isLeaf,
         bool isRoot): _minEntries(minEntries), _maxEntries(maxEntries), _dimension(dimension), 
                         _size(0),
                         _isleaf(isLeaf), _isroot(isRoot){
        if(_isleaf) _childPoly = new Polygon[_maxEntries + 1];
        else        _childNode = new Node   [_maxEntries + 1];
    }
    
/*
 *  Basic operations
 */
    void* search(Polygon *dat);
    Node* insert(Polygon &dat);
    Node* split();
    
/*
 *  Bool states
 */
    bool isleaf(){return _isleaf;}
    bool isroot(){return _isroot;}
    void toleaf(bool v){_isleaf = v;}
    void toroot(bool v){_isroot = v;}

/*
 *  Get info
 */
    size_t         getSize(){return _size;}
    Rectangle*     getMBR (){return &this->_MBR;};
    std::vector<P> get_points();

    DataType howMuchGrow(Rectangle *r);
};


/*
 * Add elements
 * ------------
 * 
 * Si no es una hoja, tiene hijos nodos.
 * Si es una hoja, tiene hijos objetos.
 * 
 * Inputs:
 *    - element: Elemento nuevo que se añade al nodo.
 * 
 */
void Node::add(Polygon &element){
    // Add to _children
    this->_childPoly[ this->_size ] = element;
    ++this->_size;

    if( this->_size < this->_maxEntries )
        this->_MBR.join(element.getMBR());
}
void Node::add(Node &element){
    // Add to _children
    this->_childNode[ this->_size ] = element;
    ++this->_size;

    if( this->_size < this->_maxEntries )
        this->_MBR.join(element.getMBR());
}
void Node::add(Node *element){
    // Add to _children
    this->_childNode[ this->_size ] = element[0];
    ++this->_size;

    if( this->_size < this->_maxEntries )
        this->_MBR.join(element[0].getMBR());
}


/*
 * Reset
 * ------------
 * 
 * Eliminar elementos del nodo.
 * 
 */
void Node::reset(){
    this->_MBR = Rectangle(this->_dimension);
    /*
    delete _childPoly;
    delete _childNode;
    if(this->_isleaf) _childPoly = new Polygon[_maxEntries + 1];
    else              _childNode = new Node   [_maxEntries + 1];        
    */
}


/*
 * Search in node
 * --------------
 * 
 * Inputs:
 *     - dat: Objeto a buscar.
 * 
 * Output:
 *     - return: Puntero a nodo
 */
void* Node::search(Polygon *dat){
    // Nodo hoja: Es posible tener dat.
    // Si no se encuentra dat, es posible crear en esta posición
    if (this->_isleaf){
        return this;
    }

    // Si no es una hoja: Busca el mejor subnodo
    else{
        // Buscar si esta dentro de algun children
        for(int i = 0; i<this->_size; ++i){
            if( dat->iswithin( this->_childNode[i].getMBR() ) )
                return this->_childNode[i].search(dat);
        }

        // Si no esta en ningun children
        // Indica en que nodo se podria añadir dat
        DataType grow;
        DataType minGrow = INF;
        int idMinGrow;
        for(int i = 0; i<this->_size; ++i){
            grow = dat->howMuchGrow( this->_childNode[i].getMBR() );
            if(minGrow>grow){
                minGrow = grow;
                idMinGrow = i;
            }
        }
        return this->_childNode[idMinGrow].search(dat);
    }

}


/*
 * Insert in node
 * --------------
 * 
 * Inputs:
 *     - dat: Objeto a insertar.
 * 
 */
Node* Node::insert(Polygon &dat){
    // Nodo hijo obtenido de un split 
    Node *childSplit = NULL;

    // Si es hoja, agrega el elemento a _childPoly.
    if (this->_isleaf){
        this->add( dat );
    }
    // Si no es hoja, agrega el elemento a _childNode.
    else{
        bool allocated = false;
        int i = 0;
        
        // Buscar hijo donde insertar
        // Primero: Nodo en donde entra
        while( i<_size && !allocated ){
            // Verifica si dat está dentro del MBR de nodo hijo
            if ( dat.iswithin( this->_childNode[i].getMBR() ) ){
                childSplit = this->_childNode[i].insert( dat );
                allocated = true;
            }
        }

        // Segundo: Nodo mas cercano
        if( !allocated ){
            DataType minGrow = INF;
            DataType childGrow;
            int idChild;
            for(i=0; i<_size; ++i){
                childGrow = dat.howMuchGrow( this->_childNode[i].getMBR() );
                if( minGrow > childGrow ){
                    minGrow = childGrow;
                    idChild = i;
                }
            }
            childSplit = this->_childNode[idChild].insert( dat );
        }
    }

    // Si existe un hijo split, se añade al nodo
    if( childSplit!=NULL ){
        this->add( childSplit );
    }

    // Si se llena el nodo: Split!
    if (_size == _maxEntries) return this->split();
    else                      return NULL;
}


/*
 * Split
 * 
 */
Node* Node::split(){
    int extreme1, extreme2;
    DataType growL1, growL2;
    
    if(_isleaf){
        //
        // Salvar datos
        // ---------------------------
        Polygon * childSaved = new Polygon[_maxEntries + 1];
        std::copy(_childPoly,_childPoly+_size,childSaved);
        size_t length = this->_size;

        // Sin datos
        this->_size = 0;

        //
        // Buscar hijos extremos
        // ---------------------------
        endsRectangles(childSaved,_size, extreme1, extreme2);

        //
        // Salvar datos
        // ---------------------------
        Node childSplit = Node(_minEntries,_maxEntries,_dimension,_isleaf,_isroot);
        this     ->add(childSaved[extreme1]);   // L1
        childSplit.add(childSaved[extreme2]);   // L2
        
        for(int i=0; i<length; ++i){
            if(i!=extreme1 && i!=extreme2 ){
                // PASO 0: Si algun nodo se queda vacio
                if( this     ->_size     == _maxEntries-_minEntries )
                    childSplit.add(childSaved[i]);
                if( childSplit.getSize() == _maxEntries-_minEntries )
                    this     ->add(childSaved[i]);

                // PASO 1: Menor crecimiento de nodos
                growL1 = childSaved[i].howMuchGrow( this     ->getMBR()  );
                growL2 = childSaved[i].howMuchGrow( childSplit.getMBR()  );

                if      (growL1<growL2) this     ->add(childSaved[i]);
                else if (growL1>growL2) childSplit.add(childSaved[i]);
                
                // PASO 2: Menor cantidad de elementos
                else{
                    if( this->_size < childSplit.getSize() )
                        this     ->add(childSaved[i]);
                    else
                        childSplit.add(childSaved[i]);
                }
            }
        }


        Node *ptrChildSplit = &childSplit;
        
        return ptrChildSplit;
    }
    else{
        //
        // Salvar datos
        // ---------------------------
        Node * childSaved = new Node[_maxEntries + 1];
        std::copy(_childNode,_childNode+_size,childSaved);
        size_t length = this->_size;

        // Sin datos
        this->_size = 0;

        //
        // Buscar hijos extremos
        // ---------------------------
        endsRectangles(childSaved,_size, extreme1, extreme2);

        
        //
        // Salvar datos
        // ---------------------------
        Node childSplit = Node(_minEntries,_maxEntries,_dimension,_isleaf,_isroot);
        this     ->add(childSaved[extreme1]);   // L1
        childSplit.add(childSaved[extreme2]);   // L2
        
        for(int i=0; i<length; ++i){
            if(i!=extreme1 && i!=extreme2 ){
                // PASO 0: Si algun nodo se queda vacio
                if( this     ->_size     == _maxEntries-_minEntries )
                    childSplit.add(childSaved[i]);
                if( childSplit.getSize() == _maxEntries-_minEntries )
                    this     ->add(childSaved[i]);

                // PASO 1: Menor crecimiento de nodos
                growL1 = childSaved[i].howMuchGrow( this     ->getMBR()  );
                growL2 = childSaved[i].howMuchGrow( childSplit.getMBR()  );

                if      (growL1<growL2) this     ->add(childSaved[i]);
                else if (growL1>growL2) childSplit.add(childSaved[i]);
                
                // PASO 2: Menor cantidad de elementos
                else{
                    if( this->_size < childSplit.getSize() )
                        this     ->add(childSaved[i]);
                    else
                        childSplit.add(childSaved[i]);
                }
            }
        }

        Node *ptrChildSplit = &childSplit;
        
        return ptrChildSplit;
    }
}


/*
 * Get points (Get MBR ends)
 * 
 * Retorna los extremos del MBR
 * 
 */
std::vector<P> Node::get_points(){
    return this->_MBR.getEnds();
}


/*
 * How much does the rectangle grow?
 * 
 * Retorna el aumento de area del rectangulo si se añade el poligono.
 * 
 */
DataType Node::howMuchGrow(Rectangle *r){
    Rectangle newR = this->_MBR.getJoin(r[0]);
    return newR.area() - r->area();
}



/*
 * Get ends rectangles
 * 
 */
void Node::endsRectangles(Rectangle* rects,size_t &length, int &extreme1, int &extreme2){
    Rectangle fusion;
    DataType maxArea = -INF;
    DataType fusionArea;

    for(int i=0; i<length-1; ++i){
        for(int j = i+1; j<length;++j){
            fusion = rects[i].getJoin( rects[j] );
            fusionArea = fusion.area();
            if( maxArea<fusionArea ){
                maxArea = fusionArea;
                extreme1 = i;
                extreme2 = j;
            }
        }
    }
}
void Node::endsRectangles(Polygon* poly,size_t &length, int &extreme1, int &extreme2){
    Rectangle fusion;
    DataType maxArea = -INF;
    DataType fusionArea;
    Rectangle *rect1, *rect2;

    for(int i=0; i<length-1; ++i){
        for(int j = i+1; j<length;++j){
            rect1 = poly[i].getMBR();
            rect2 = poly[j].getMBR();
            fusion = rect1->getJoin( rect2 );
            fusionArea = fusion.area();
            if( maxArea<fusionArea ){
                maxArea = fusionArea;
                extreme1 = i;
                extreme2 = j;
            }
        }
    }
}
void Node::endsRectangles(Node* nodo,size_t &length, int &extreme1, int &extreme2){
    Rectangle fusion;
    DataType maxArea = -INF;
    DataType fusionArea;
    Rectangle *rect1, *rect2;

    for(int i=0; i<length-1; ++i){
        for(int j = i+1; j<length;++j){
            rect1 = nodo[i].getMBR();
            rect2 = nodo[j].getMBR();
            fusion = rect1->getJoin( rect2 );
            fusionArea = fusion.area();
            if( maxArea<fusionArea ){
                maxArea = fusionArea;
                extreme1 = i;
                extreme2 = j;
            }
        }
    }
}











/*
 * Class R-Tree
 * ...........................................................................
 */
class RTree
{
private:
    Node* _root;
    int _minEntries;
    int _maxEntries;
    int _dimension;

public:
    RTree(): _minEntries(0), _maxEntries(0), _dimension(0){};
    RTree(int minEntries, 
          int maxEntries, 
          int dimension): _minEntries(minEntries), 
                          _maxEntries(maxEntries), 
                          _dimension ( dimension){
        _root = NULL;
    }

    bool  insert(Polygon *dat);
    void* search(Polygon *dat){
        return (_root == NULL)? NULL : _root->search(dat);
    }

};

bool RTree::insert(Polygon *dat){
    /*
    // Si esta vacio
    if (_root == NULL){
        _root = new Node(_minEntries,_maxEntries,_dimension);
    }
    // Si no esta vacio
    else{
        // Aun no se llena la raiz
        if(_root->isleaf()){
            // Deberia funcionar con root lleno o semilleno o vacio
            _root->insert(dat);
        }
        // 
        else{
            // Rutina normal 
        }
    }
    */
}
















#endif // RTREE_H


int main(int argc, char const *argv[]) {

    //Node<Object> a(2,5,2);

    std::cout << ":c" << std::endl;

    return 0;
}

