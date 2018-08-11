#include <iostream>
#include <math.h>
#include <vector>
#include "RTree.h"

using namespace std;

typedef int dtype;

int main() {

    RTree<dtype> *tree = new RTree<dtype>(2,5);

    vector<Point<dtype> > arr1;
    arr1.push_back(*(new Point<dtype>(414,214)));
    arr1.push_back(*(new Point<dtype>(389,381)));
    arr1.push_back(*(new Point<dtype>(810,516)));
    arr1.push_back(*(new Point<dtype>(819,188)));
    arr1.push_back(*(new Point<dtype>(545,380)));
    arr1.push_back(*(new Point<dtype>(284,410)));
    arr1.push_back(*(new Point<dtype>(620,634)));
    arr1.push_back(*(new Point<dtype>(934,428)));
    arr1.push_back(*(new Point<dtype>(480,446)));
    arr1.push_back(*(new Point<dtype>(713,583)));
    arr1.push_back(*(new Point<dtype>(1130,453)));
    arr1.push_back(*(new Point<dtype>(707,257)));

    // Polygon<dtype>* p1 = new Polygon<dtype>(arr1, 1);
    // Polygon<dtype>* p2 = new Polygon<dtype>(arr2, 2);

    for (size_t i = 0; i < arr1.size(); i += 1) {
        Polygon<dtype>* p = new Polygon<dtype>(arr1, i + 1);
        tree->insert(p);
    }

    // cout << tree->getDistance(p1,p2) << endl;
    tree->print();
    // cout << " ======= " << endl;
    // Point<dtype> searchPoint(9, 8);
    // tree->nearestSearch(searchPoint, 0);

    return 0;
}
