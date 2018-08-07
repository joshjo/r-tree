#ifndef Point_cpp
#define Point_cpp

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
};
#endif
