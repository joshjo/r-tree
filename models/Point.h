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

        void print() {
            cout << "(" << x << ", " << y << ")";
        }
};
#endif
