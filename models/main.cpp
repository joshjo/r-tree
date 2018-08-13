#include <iostream>
#include <math.h>
#include <vector>
#include "RTreeplus.h"
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
    // arr1.push_back(*(new Point<dtype>(486,213  )));
    // arr1.push_back(*(new Point<dtype>(414, 214))); // 1
    // arr1.push_back(*(new Point<dtype>(389, 381))); // 2
    // arr1.push_back(*(new Point<dtype>(810, 516))); // 3
    // arr1.push_back(*(new Point<dtype>(819, 188))); // 4
    // arr1.push_back(*(new Point<dtype>(545, 380))); // 5
    // arr1.push_back(*(new Point<dtype>(284, 410))); // 6
    // arr1.push_back(*(new Point<dtype>(620, 634))); // 7
    // arr1.push_back(*(new Point<dtype>(934, 428))); // 8
    // arr1.push_back(*(new Point<dtype>(480, 446))); // 9
    // arr1.push_back(*(new Point<dtype>(420, 446))); // 10
    // arr1.push_back(*(new Point<dtype>(713, 583))); // 11
    // arr1.push_back(*(new Point<dtype>(130, 453))); // 12
    // arr1.push_back(*(new Point<dtype>(707, 257))); // 13
    // arr1.push_back(*(new Point<dtype>(624, 353))); // 14
    // arr1.push_back(*(new Point<dtype>(550, 358))); // 15
    // arr1.push_back(*(new Point<dtype>(450, 258))); // 16
    // arr1.push_back(*(new Point<dtype>(620, 351))); // 17
    // arr1.push_back(*(new Point<dtype>(610, 321))); // 18
    // arr1.push_back(*(new Point<dtype>(611, 322))); // 19

    // arr1.push_back(*(new Point<dtype>(556,333  )));
    // arr1.push_back(*(new Point<dtype>(454,477  )));
    // arr1.push_back(*(new Point<dtype>(861,482  )));
    // arr1.push_back(*(new Point<dtype>(792,254  )));
    // arr1.push_back(*(new Point<dtype>(545,211  )));
    // arr1.push_back(*(new Point<dtype>(415,295  )));
    // arr1.push_back(*(new Point<dtype>(425,503  )));
    // arr1.push_back(*(new Point<dtype>(816,553  )));
    // arr1.push_back(*(new Point<dtype>(851,228  )));
    // arr1.push_back(*(new Point<dtype>(696,115  )));
    // arr1.push_back(*(new Point<dtype>(327,119  )));
    // arr1.push_back(*(new Point<dtype>(247,424  )));
    // arr1.push_back(*(new Point<dtype>(455,609  )));
    // arr1.push_back(*(new Point<dtype>(462,408  )));
    // arr1.push_back(*(new Point<dtype>(300,334  )));
    // arr1.push_back(*(new Point<dtype>(188,224  )));
    // arr1.push_back(*(new Point<dtype>(151,411  )));
    // arr1.push_back(*(new Point<dtype>(377,513  )));
    // arr1.push_back(*(new Point<dtype>(266,344  )));
    // arr1.push_back(*(new Point<dtype>(191,278  )));

    // arr1.push_back(*(new Point<dtype>(114,273  )));
    // arr1.push_back(*(new Point<dtype>(239,571  )));
    // arr1.push_back(*(new Point<dtype>(427,629  )));
    // arr1.push_back(*(new Point<dtype>(573,715  )));
    // arr1.push_back(*(new Point<dtype>(929,430  )));
    // arr1.push_back(*(new Point<dtype>(931,304  )));
    // arr1.push_back(*(new Point<dtype>(928,224  )));
    // arr1.push_back(*(new Point<dtype>(948,160  )));
    // arr1.push_back(*(new Point<dtype>(1008,160  )));
    // arr1.push_back(*(new Point<dtype>(1008,274  )));
    // arr1.push_back(*(new Point<dtype>(236,567  )));
    // arr1.push_back(*(new Point<dtype>(234,730  )));
    // arr1.push_back(*(new Point<dtype>(368,715  )));
    // arr1.push_back(*(new Point<dtype>(106,645  )));
    // arr1.push_back(*(new Point<dtype>(99,414  )));
    // arr1.push_back(*(new Point<dtype>(254,459  )));
    // arr1.push_back(*(new Point<dtype>(295,498  )));
    // arr1.push_back(*(new Point<dtype>(230,529  )));
    // arr1.push_back(*(new Point<dtype>(172,432  )));
    // arr1.push_back(*(new Point<dtype>(210,374  )));
    // arr1.push_back(*(new Point<dtype>(271,371  )));
    // arr1.push_back(*(new Point<dtype>(311,435  )));
    // arr1.push_back(*(new Point<dtype>(311,462  )));
    // arr1.push_back(*(new Point<dtype>(254,498  )));
    // arr1.push_back(*(new Point<dtype>(231,596  )));
    // arr1.push_back(*(new Point<dtype>(251,678  )));
    // arr1.push_back(*(new Point<dtype>(385,678  )));
    // arr1.push_back(*(new Point<dtype>(694,716  )));
    // arr1.push_back(*(new Point<dtype>(748,730  )));
    // arr1.push_back(*(new Point<dtype>(919,725  )));
    // arr1.push_back(*(new Point<dtype>(929,688  )));
    // arr1.push_back(*(new Point<dtype>(936,580  )));
    // arr1.push_back(*(new Point<dtype>(880,503  )));
    // arr1.push_back(*(new Point<dtype>(909,316  )));
    // arr1.push_back(*(new Point<dtype>(914,177  )));
    // arr1.push_back(*(new Point<dtype>(919,113  )));
    // arr1.push_back(*(new Point<dtype>(99,121  )));
    // arr1.push_back(*(new Point<dtype>(51,199  )));
    // arr1.push_back(*(new Point<dtype>(132,357  )));
    // arr1.push_back(*(new Point<dtype>(423,391  )));
    // arr1.push_back(*(new Point<dtype>(593,415  )));
    // arr1.push_back(*(new Point<dtype>(603,104  )));
    // arr1.push_back(*(new Point<dtype>(720,76  )));
    // arr1.push_back(*(new Point<dtype>(535,51  )));
    // arr1.push_back(*(new Point<dtype>(436,88  )));
    // arr1.push_back(*(new Point<dtype>(299,34  )));
    // arr1.push_back(*(new Point<dtype>(271,87  )));
    // arr1.push_back(*(new Point<dtype>(545,59  )));
    // arr1.push_back(*(new Point<dtype>(728,48  )));
    // arr1.push_back(*(new Point<dtype>(920,79  )));
    // arr1.push_back(*(new Point<dtype>(1038,156  )));
    // arr1.push_back(*(new Point<dtype>(1079,287  )));
    // arr1.push_back(*(new Point<dtype>(1048,387  )));
    // arr1.push_back(*(new Point<dtype>(976,453  )));
    // arr1.push_back(*(new Point<dtype>(967,549  )));
    // arr1.push_back(*(new Point<dtype>(671,395  )));

    // arr1.push_back(*(new Point<dtype>(388,131  ))); // 1
    // arr1.push_back(*(new Point<dtype>(246,375  ))); // 2
    // arr1.push_back(*(new Point<dtype>(595,339  ))); // 3
    // arr1.push_back(*(new Point<dtype>(533,158  ))); // 4
    // arr1.push_back(*(new Point<dtype>(295,227  ))); // 5
    // arr1.push_back(*(new Point<dtype>(905,294  ))); // 6

    // arr1.push_back(*(new Point<dtype>(546,248  )));
    // arr1.push_back(*(new Point<dtype>(754,371  )));
    // arr1.push_back(*(new Point<dtype>(440,229  )));
    // arr1.push_back(*(new Point<dtype>(734,108  ))); // 7
    // arr1.push_back(*(new Point<dtype>(196,127  ))); // 8
    // arr1.push_back(*(new Point<dtype>(345,470  ))); // 9
    // arr1.push_back(*(new Point<dtype>(808,95  )));  // 10

    for(int i = 0; i < val; i++)
        // for(int j = 0;j<10;j++) {
        arr1.push_back(*(new Point<dtype>(i / 10, i % 10)));
        // }

    for (size_t i = 0; i < arr1.size(); i += 1) {
        Polygon<dtype>* p = new Polygon<dtype>(arr1[i], i + 1);
        tree->insert(p);
        // if (i == 27) {
        //     tree->print();
        //     cout << "===================" << endl;
        // }
    }


    // cout << tree->getDistance(p1,p2) << endl;
    // tree->print();
    cout << tree->graphviz() << endl;
    // cout << tree->get_json_string() << endl;
    // cout << " ======= " << endl;
    // Point<dtype> searchPoint(9, 8);
    // tree->nearestSearch(searchPoint, 0);

    return 0;
}
