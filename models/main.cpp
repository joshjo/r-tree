#include <iostream>
#include <math.h>
#include <vector>
#include "RTree.h"
#include <sstream>

using namespace std;

typedef int dtype;

int main(int argc, char *argv[]) {
    int val = 10;
    if (argc >= 2)
    {
        std::istringstream iss( argv[1] );
        // int val;

        if ((iss >> val) && iss.eof()) // Check eofbit
        {
            // Conversion successful
        }
    }

    RTree<dtype> *tree = new RTree<dtype>(5, 2);

    vector<Point<dtype> > arr1;
    arr1.push_back(*(new Point<dtype>(130, 187))); // 1
    arr1.push_back(*(new Point<dtype>( 59, 298))); // 2
    arr1.push_back(*(new Point<dtype>(274, 301))); // 3
    arr1.push_back(*(new Point<dtype>(217, 149))); // 4

    vector<Point<dtype> > arr2;
    arr1.push_back(*(new Point<dtype>(302, 205))); // 5
    arr1.push_back(*(new Point<dtype>(155, 322))); // 6
    arr1.push_back(*(new Point<dtype>( 87, 158))); // 7

    vector<Point<dtype> > arr3;
    arr1.push_back(*(new Point<dtype>( 46, 236))); // 8
    arr1.push_back(*(new Point<dtype>(166, 281))); // 9
    arr1.push_back(*(new Point<dtype>(244, 324))); // 10

    // for(int i = 0; i < val; i++)
    //     // for(int j = 0;j<10;j++) {
    //     arr1.push_back(*(new Point<dtype>(i / 10, i % 10)));
    //     // }

    // for (size_t i = 0; i < arr1.size(); i += 1) {
    Polygon<dtype>* p = new Polygon<dtype>(arr1, 1);
    tree->insert(p);
    Polygon<dtype>* p2 = new Polygon<dtype>(arr2, 2);
    tree->insert(p2);
    Polygon<dtype>* p3 = new Polygon<dtype>(arr3, 3);
    tree->insert(p3);
    // }


    Point<dtype> point (100, 100);

    tree->nearestSearch(point, 3);

    // cout << tree->getDistance(p1,p2) << endl;

    // cout << tree->graphviz() << endl;
    // cout << tree->get_json_string() << endl;
    // cout << " ======= " << endl;
    // Point<dtype> searchPoint(9, 8);
    // tree->nearestSearch(searchPoint, 0);

}
