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

        void print() {
            cout << "(" << x << ", " << y << ")";
        }
};
#endif
