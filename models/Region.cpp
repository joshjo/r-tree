#include "Point.cpp"

using namespace std;

class Region{
public:
	Region();
	Region(int,Point,Point);
	int getId();
	void setId(int);
	Point getMin();
	void setMin(Point);
	Point getMax();
	void setMax(Point);
private:
	int id;
	Point min = Point();
	Point max = Point();
};
Region::Region(int pId,Point pMin, Point pMax){
	this->id = pId;
	this->min = pMin;
	this->max = pMax;
}
int Region::getId(){
	return this->id;
}
void Region::setId(int pId){
	this->id = pId;
}
Point Region::getMin(){
	return this->min;
}
void Region::setMin(Point pMin){
	this->min = pMin;
}
Point Region::getMax(){
	return this->max;
}
void Region::setMax(Point pMax){
	this->max = pMax;
}