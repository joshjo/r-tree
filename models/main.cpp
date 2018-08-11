#include <iostream>
#include <math.h>
#include <vector>
#include "RTree.h"

using namespace std;

typedef int dtype;

int main() {

    RTree<dtype> *tree = new RTree<dtype>(2,5);

    vector<Point<dtype> > arr1;
    arr1.push_back(*(new Point<dtype>(0, 4)));
    arr1.push_back(*(new Point<dtype>(0, 7)));
    arr1.push_back(*(new Point<dtype>(3, 4)));
    vector<Point<dtype>> arr2;
    arr2.push_back(*(new Point<dtype>(5, 4)));
    arr2.push_back(*(new Point<dtype>(9, 4)));
    arr2.push_back(*(new Point<dtype>(5, 0)));
    arr2.push_back(*(new Point<dtype>(9, 0)));    

    Polygon<dtype>* p1 = new Polygon<dtype>(arr1, 1);
    Polygon<dtype>* p2 = new Polygon<dtype>(arr2, 2);
    
    cout << tree->getDistance(p1,p2) << endl;
    //tree->print();
    // cout << " ======= " << endl;
    // Point<dtype> searchPoint(9, 8);
    // tree->nearestSearch(searchPoint, 0);

    return 0;
}
