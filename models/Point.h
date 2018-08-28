#ifndef Point_cpp
#define Point_cpp

#include "includes.h"

template <class T>
class Point{
public:
    T x;
    T y;
    Point(){}
    Point(T px,T py){
        this->x = px;
        this->y = py;
    }

    string to_string(
        string initSymbol = "[",
        string endSymbol = "]",
        string separator = ","
    ) {
        string str;
        str += initSymbol + std::to_string(x) + "," + std::to_string(y) + endSymbol + separator;
        return str;
    }

    Point<T> getOrthogonal(){
        return Point<T>(-y,x);
    }

    T length2(){
        return this->x*this->x + this->y*this->y;
    }

    double length(){
        return sqrt( (double)this->length2() );
    }

    Point<T> operator-(const Point<T>& other){;
        return Point<T>(this->x - other.x,
                        this->y - other.y);
    }

    Point<T> operator+(const Point<T>& other){;
        return Point<T>(this->x + other.x,
                        this->y + other.y);
    }

    Point<T> operator*(const Point<T>& other){;
        return Point<T>(this->x * other.x,
                        this->y * other.y);
    }

    Point<T> operator*(const int scalar){;
        return Point<T>(this->x * scalar,
                        this->y * scalar);
    }

    Point<T> operator*(const double scalar){;
        return Point<T>( ((double)this->x)*scalar,
                         ((double)this->y)*scalar);
    }

    void print() {
        cout << "(" << x << ", " << y << ")";
    }
};

/*
int distanceSide(Point<int> &P, Point<int> &A, Point<int> &B){
        Point<int> a = A - B;
        Point<int> b = a.getOrthogonal();
        Point<int> minDistance;

        double coeff = (double)(a.y*b.x - a.x*b.y);
        double beta = (double)( a.x*(P.y - A.y) - a.y*(P.x - A.x) );

        beta = beta / coeff;

        Point<int> C( ((double)P.x) + beta * ((double)b.x),
                      ((double)P.y) + beta * ((double)b.y)  ); 
        Point<int> PC = P - C; 
        
        cout << "A: "; A.print(); cout << endl;
        cout << "B: "; B.print(); cout << endl;
        cout << "P: "; P.print(); cout << endl;
        cout << "AB: "; a.print(); cout << endl;
        cout << "ABt: "; b.print(); cout << endl;
        cout << "coeff:" << coeff << endl;
        cout << "beta:" << beta << endl;

        cout << "C: "; C.print(); cout << endl;



        // La distancia es perpendicular
        if( A.x < C.x && C.x < B.x ) return PC.length2();
        if( B.x < C.x && C.x < A.x ) return PC.length2();

        //  La distancia no es perpendicular
        Point<int> PA = P - A;
        Point<int> PB = P - B;

        int disPA = PA.length2();
        int disPB = PB.length2();

        if( disPA<disPB ) return disPA;
        else              return disPB;
    }
*/
#endif
