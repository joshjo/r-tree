#include "Point.cpp"

using namespace std;

class Polygon{
public:
	Polygon();
	Polygon(int, vector<Point>);
	int getId();
	void setId(int);
	vector<Point> getVectorPoints();
	void setVectorPoint(vector<Point>);
private:
	int id;
	vector<Point> pv;
};
Polygon::Polygon(){}
Polygon::Polygon(int pId, vector<Point> pVp){
	this->id = pId;
	this->pv = pVp;
}
int Polygon::getId(){
	return this->id;
}
void Polygon::setId(int pId){
	this->id = pId;
}
vector<Point> Polygon::getVectorPoints(){
	return this->pv;
}
void Polygon::setVectorPoint(vector<Point> pVp){
	this->pv = pVp;
}