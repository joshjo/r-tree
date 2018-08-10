#include <iostream>
#include <math.h>
#include <vector>
#include "RTree.h"

using namespace std;

typedef int dtype;

int main() {

    RTree<dtype> *tree = new RTree<dtype>(2,5);

    vector<Point<dtype> > arr;
    arr.push_back(*(new Point<dtype>(2, 3)));
    arr.push_back(*(new Point<dtype>(4, 6)));
    arr.push_back(*(new Point<dtype>(9, 6)));
    arr.push_back(*(new Point<dtype>(9, 2)));
    arr.push_back(*(new Point<dtype>(8, 3)));
    arr.push_back(*(new Point<dtype>(1, 8)));
    arr.push_back(*(new Point<dtype>(5, 1)));
    arr.push_back(*(new Point<dtype>(5, 4)));
    arr.push_back(*(new Point<dtype>(6, 3)));
    arr.push_back(*(new Point<dtype>(7, 2)));

    for(size_t i = 0; i < arr.size(); i += 1) {
        Polygon<dtype>* p = new Polygon<dtype>(arr[i], i + 1);
        tree->insert(p);
    }
    cout << tree->get_json_string() << endl;
    // tree->print();
    // cout << " ======= " << endl;
    Point<dtype> searchPoint(9, 8);
    tree->nearestSearch(searchPoint, 0);

    return 0;
}
