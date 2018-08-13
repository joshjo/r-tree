#include <iostream>
#include <math.h>
#include <vector>
#include "RTreeplus.h"

using namespace std;

typedef int dtype;

int main() {

    RTree<dtype> *tree = new RTree<dtype>(5, 2);

    vector<Point<dtype> > arr1;
    arr1.push_back(*(new Point<dtype>(2,2  ))); // 1
    arr1.push_back(*(new Point<dtype>(3,4  ))); // 2
    arr1.push_back(*(new Point<dtype>(4,2  ))); // 3
    arr1.push_back(*(new Point<dtype>(6,2  ))); // 4
    arr1.push_back(*(new Point<dtype>(6,4  ))); // 5
    arr1.push_back(*(new Point<dtype>(8,5  ))); // 6

    arr1.push_back(*(new Point<dtype>(9,8  )));

    /*for(int i = 0; i < 10;i++)
        for(int j = 0;j<10;j++)
            arr1.push_back(*(new Point<dtype>(i,j  )));*/
    //arr1.push_back(*(new Point<dtype>(9,8  )));
    // arr1.push_back(*(new Point<dtype>(440,229  )));
    // arr1.push_back(*(new Point<dtype>(734,108  ))); // 7
    // arr1.push_back(*(new Point<dtype>(196,127  ))); // 8
    // arr1.push_back(*(new Point<dtype>(345,470  ))); // 9
    // arr1.push_back(*(new Point<dtype>(808,95  )));  // 10

    for (size_t i = 0; i < arr1.size(); i += 1) {
        Polygon<dtype>* p = new Polygon<dtype>(arr1[i], i + 1);
        cout << "region id " << tree->insert(p) << endl;
    }
    Point<dtype> min(4,2);
    //Point<dtype> max(8,5);
    //Rectangle<dtype>* r = new Rectangle<dtype>(min,max);
    auto elements = tree->nearestSearch(min,2);
    for(auto e : elements)
        cout << e->get_id() << endl;
    // cout << tree->getDistance(p1,p2) << endl;
    tree->print();
    // cout << tree->get_json_string() << endl;
    // cout << " ======= " << endl;
    // Point<dtype> searchPoint(9, 8);
    // tree->nearestSearch(searchPoint, 0);

    return 0;
}