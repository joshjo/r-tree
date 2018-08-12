#include <iostream>
#include <math.h>
#include <vector>
#include "RTreeplus.h"

using namespace std;

typedef int dtype;

int main() {

    RTree<dtype> *tree = new RTree<dtype>(5, 2);

    vector<Point<dtype> > arr1;
    arr1.push_back(*(new Point<dtype>(414, 214))); // 1
    arr1.push_back(*(new Point<dtype>(389, 381))); // 2
    arr1.push_back(*(new Point<dtype>(810, 516))); // 3
    arr1.push_back(*(new Point<dtype>(819, 188))); // 4
    arr1.push_back(*(new Point<dtype>(545, 380))); // 5
    arr1.push_back(*(new Point<dtype>(284, 410))); // 6
    arr1.push_back(*(new Point<dtype>(620, 634))); // 7
    arr1.push_back(*(new Point<dtype>(934, 428))); // 8
    arr1.push_back(*(new Point<dtype>(480, 446))); // 9
    arr1.push_back(*(new Point<dtype>(420, 446))); // 10
    arr1.push_back(*(new Point<dtype>(713, 583))); // 11
    arr1.push_back(*(new Point<dtype>(130, 453))); // 12
    arr1.push_back(*(new Point<dtype>(707, 257))); // 13
    // arr1.push_back(*(new Point<dtype>(624, 353))); // 14
    // arr1.push_back(*(new Point<dtype>(550, 358))); // 15


    for (size_t i = 0; i < arr1.size(); i += 1) {
        Polygon<dtype>* p = new Polygon<dtype>(arr1[i], i + 1);
        tree->insert(p);
    }


    // cout << tree->getDistance(p1,p2) << endl;
    tree->print();
    // cout << tree->get_json_string() << endl;
    // cout << " ======= " << endl;
    // Point<dtype> searchPoint(9, 8);
    // tree->nearestSearch(searchPoint, 0);

    return 0;
}