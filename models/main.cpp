#include <iostream>
#include <math.h>
#include "RTree.h"

using namespace std;

typedef int dtype;

int main() {
	
    RTree<dtype> *tree = new RTree<dtype>(2,5);
    int count = 1;
    
    Polygon<dtype> *poly = new Polygon<dtype>(*(new Point<dtype>(2,3)),count++);
    Polygon<dtype> *poly2 = new Polygon<dtype>(*(new Point<dtype>(4,6)),count++);
    tree->insert(poly);  
  /*Polygon<dtype> *poly2 = new Polygon<dtype>(3,4,count++);
    Polygon<dtype> *poly3 = new Polygon<dtype>(3,1,count++);

    tree->insert(poly);
    tree->insert(poly1);
    tree->insert(poly2);
    tree->insert(poly3);

    Polygon<string> *poly4 = new Polygon<string>(6,9,"E");
    tree->insert(poly4);
    Polygon<string> *poly5 = new Polygon<string>(3,3,"F");
    tree->insert(poly5);
    Polygon<string> *poly6 = new Polygon<string>(9,2,"G");
    tree->insert(poly6);
    Polygon<string> *poly7 = new Polygon<string>(8,3,"H");
    tree->insert(poly7);
    Polygon<string> *poly8 = new Polygon<string>(9,6,"I");
    tree->insert(poly8);
    Polygon<string> *poly9 = new Polygon<string>(9,6,"J");
    tree->insert(poly9);
    Polygon<string> *poly10 = new Polygon<string>(9,6,"K");
    tree->insert(poly10);
    Polygon<string> *poly11 = new Polygon<string>(9,6,"L");
    tree->insert(poly11);
    Polygon<string> *poly12 = new Polygon<string>(11,8,"M");
    tree->insert(poly12);
    Polygon<string> *poly13 = new Polygon<string>(13,9,"N");
    tree->insert(poly13);

    Polygon<string> *poly14 = new Polygon<string>(13,8,"Ñ");
    tree->insert(poly14);
    Polygon<string> *poly15 = new Polygon<string>(14,9,"O");
    tree->insert(poly15);
    Polygon<string> *poly16 = new Polygon<string>(15,8,"P");
    tree->insert(poly16);

    Polygon<string> *poly17 = new Polygon<string>(15,11,"Q");
    tree->insert(poly17);

    tree->print();
	 */
    return 0;
}
