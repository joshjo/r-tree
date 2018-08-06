#ifndef Point_cpp
#define Point_cpp
class Point{
private:
	int x;
	int y;
public:
	Point();
	Point(int,int);
	int getX();
	void setX(int);
	int getY();
	void setY(int);
};
Point::Point(){}
Point::Point(int px,int py){
	this->x = px;
	this->y = py;
}
int Point::getX(){
	return this->x;
}
void Point::setX(int pX){
	this->x = pX;
}
int Point::getY(){
	return this->y;
}
void Point::setY(int pY){
	this->y = pY;
}
#endif 