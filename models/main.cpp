#include <iostream>
#include <math.h>
#include "RTree.h"

using namespace std;

typedef int cid;

cid lid = 1;

int main() {

    Polygon<cid> *poly = new Polygon<cid>(2,3, lid++);
    Polygon<cid> *poly1 = new Polygon<cid>(2,6, lid++);
    Polygon<cid> *poly2 = new Polygon<cid>(3,4, lid++);
    Polygon<cid> *poly3 = new Polygon<cid>(3,1, lid++);
    RTree<cid> *tree = new RTree<cid>(2,5);
    tree->insert(poly);
    tree->insert(poly1);
    tree->insert(poly2);
    tree->insert(poly3);

    Polygon<cid> *poly4 = new Polygon<cid>(6,9,lid++);
    tree->insert(poly4);
    Polygon<cid> *poly5 = new Polygon<cid>(3,3,lid++);
    tree->insert(poly5);
    Polygon<cid> *poly6 = new Polygon<cid>(9,2,lid++);
    tree->insert(poly6);
    Polygon<cid> *poly7 = new Polygon<cid>(8,3,lid++);
    tree->insert(poly7);
    Polygon<cid> *poly8 = new Polygon<cid>(9,6,lid++);
    tree->insert(poly8);
    Polygon<cid> *poly9 = new Polygon<cid>(9,6,lid++);
    tree->insert(poly9);
    Polygon<cid> *poly10 = new Polygon<cid>(9,6,lid++);
    tree->insert(poly10);
    Polygon<cid> *poly11 = new Polygon<cid>(9,6,lid++);
    tree->insert(poly11);
    Polygon<cid> *poly12 = new Polygon<cid>(11,8,lid++);
    tree->insert(poly12);
    Polygon<cid> *poly13 = new Polygon<cid>(13,9,lid++);
    tree->insert(poly13);

    Polygon<cid> *poly14 = new Polygon<cid>(13,8,lid++);
    tree->insert(poly14);
    Polygon<cid> *poly15 = new Polygon<cid>(14,9,lid++);
    tree->insert(poly15);
    Polygon<cid> *poly16 = new Polygon<cid>(15,8,lid++);
    tree->insert(poly16);

    Polygon<cid> *poly17 = new Polygon<cid>(15,11,lid++);
    tree->insert(poly17);

    tree->print();
    return 0;
}
