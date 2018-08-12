#include "server_http.hpp"
#include <iostream>
#include <typeinfo>
#include <string>
#include <utility>
//Models for rtree
// #include "models/Polygon.cpp"
// #include "models/Region.cpp"
#include "models/RTreeplus.h"
// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

using namespace std;
//#define pair<int, int> point
// Added for the json-example:
using namespace boost::property_tree;

typedef int dtype;
typedef Point<dtype> P;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

int main() {
    // HTTP-server at port 8080 using 1 thread
    // Unless you do more heavy non-threaded processing in the resources,
    // 1 thread is usually faster than several threads
    HttpServer server;
    server.config.port = 8090;
    RTree<dtype> * tree = new RTree<dtype>(5, 2);

    vector<Point<dtype> > arr1;
    arr1.push_back(*(new Point<dtype>(556,333  )));
    arr1.push_back(*(new Point<dtype>(454,477  )));
    arr1.push_back(*(new Point<dtype>(861,482  )));
    arr1.push_back(*(new Point<dtype>(792,254  )));
    arr1.push_back(*(new Point<dtype>(545,211  )));
    arr1.push_back(*(new Point<dtype>(415,295  )));
    arr1.push_back(*(new Point<dtype>(425,503  )));
    arr1.push_back(*(new Point<dtype>(816,553  )));
    arr1.push_back(*(new Point<dtype>(851,228  )));
    arr1.push_back(*(new Point<dtype>(696,115  )));
    arr1.push_back(*(new Point<dtype>(327,119  )));
    arr1.push_back(*(new Point<dtype>(247,424  )));
    arr1.push_back(*(new Point<dtype>(455,609  )));
    arr1.push_back(*(new Point<dtype>(462,408  )));
    arr1.push_back(*(new Point<dtype>(300,334  )));
    arr1.push_back(*(new Point<dtype>(188,224  )));
    arr1.push_back(*(new Point<dtype>(151,411  )));
    arr1.push_back(*(new Point<dtype>(377,513  )));
    arr1.push_back(*(new Point<dtype>(266,344  )));
    arr1.push_back(*(new Point<dtype>(191,278  )));

    arr1.push_back(*(new Point<dtype>(114,273  )));
    arr1.push_back(*(new Point<dtype>(239,571  )));
    arr1.push_back(*(new Point<dtype>(427,629  )));
    arr1.push_back(*(new Point<dtype>(573,715  )));
    arr1.push_back(*(new Point<dtype>(929,430  )));
    arr1.push_back(*(new Point<dtype>(931,304  )));
    arr1.push_back(*(new Point<dtype>(928,224  )));
    arr1.push_back(*(new Point<dtype>(948,160  )));
    arr1.push_back(*(new Point<dtype>(1008,160  )));
    arr1.push_back(*(new Point<dtype>(1008,274  )));
    arr1.push_back(*(new Point<dtype>(236,567  )));
    arr1.push_back(*(new Point<dtype>(234,730  )));
    arr1.push_back(*(new Point<dtype>(368,715  )));
    arr1.push_back(*(new Point<dtype>(106,645  )));
    arr1.push_back(*(new Point<dtype>(99,414  )));
    arr1.push_back(*(new Point<dtype>(254,459  )));
    arr1.push_back(*(new Point<dtype>(295,498  )));
    arr1.push_back(*(new Point<dtype>(230,529  )));
    arr1.push_back(*(new Point<dtype>(172,432  )));
    arr1.push_back(*(new Point<dtype>(210,374  )));
    arr1.push_back(*(new Point<dtype>(271,371  )));
    arr1.push_back(*(new Point<dtype>(311,435  )));
    arr1.push_back(*(new Point<dtype>(311,462  )));
    arr1.push_back(*(new Point<dtype>(254,498  )));
    arr1.push_back(*(new Point<dtype>(231,596  )));
    arr1.push_back(*(new Point<dtype>(251,678  )));
    arr1.push_back(*(new Point<dtype>(385,678  )));
    arr1.push_back(*(new Point<dtype>(694,716  )));
    arr1.push_back(*(new Point<dtype>(748,730  )));
    arr1.push_back(*(new Point<dtype>(919,725  )));
    arr1.push_back(*(new Point<dtype>(929,688  )));
    arr1.push_back(*(new Point<dtype>(936,580  )));
    arr1.push_back(*(new Point<dtype>(880,503  )));
    arr1.push_back(*(new Point<dtype>(909,316  )));
    arr1.push_back(*(new Point<dtype>(914,177  )));
    arr1.push_back(*(new Point<dtype>(919,113  )));
    arr1.push_back(*(new Point<dtype>(99,121  )));
    arr1.push_back(*(new Point<dtype>(51,199  )));
    arr1.push_back(*(new Point<dtype>(132,357  )));
    arr1.push_back(*(new Point<dtype>(423,391  )));
    arr1.push_back(*(new Point<dtype>(593,415  )));
    arr1.push_back(*(new Point<dtype>(603,104  )));
    arr1.push_back(*(new Point<dtype>(720,76  )));
    arr1.push_back(*(new Point<dtype>(535,51  )));
    arr1.push_back(*(new Point<dtype>(436,88  )));
    arr1.push_back(*(new Point<dtype>(299,34  )));
    arr1.push_back(*(new Point<dtype>(271,87  )));
    arr1.push_back(*(new Point<dtype>(545,59  )));
    arr1.push_back(*(new Point<dtype>(728,48  )));
    arr1.push_back(*(new Point<dtype>(920,79  )));
    arr1.push_back(*(new Point<dtype>(1038,156  )));
    arr1.push_back(*(new Point<dtype>(1079,287  )));
    arr1.push_back(*(new Point<dtype>(1048,387  )));
    arr1.push_back(*(new Point<dtype>(976,453  )));
    arr1.push_back(*(new Point<dtype>(967,549  )));
    arr1.push_back(*(new Point<dtype>(671,395  )));


    for (size_t i = 0; i < arr1.size(); i += 1) {
        Polygon<dtype>* p = new Polygon<dtype>(arr1[i], i + 1);
        tree->insert(p);
    }

    int count = 1;
    //Get rtree
    server.resource["^/rtree$"]["GET"] = [&tree](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        stringstream stream;
        SimpleWeb::CaseInsensitiveMultimap header;
        // string json_string = "{\"polygon\": [";
        stream << tree->get_json_string();
        response->write_get(stream,header);
    };

    //Post rtree
    server.resource["^/rtree$"]["POST"] = [&tree, &count](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        // Polygon polygon;
        vector<P> pv;
        vector<dtype> v;
        stringstream stream;
        string json_string = "";
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            ptree pt;
            read_json(request->content, pt);
            for (boost::property_tree::ptree::value_type& rowPair:pt.get_child("polygon")) {
                for (boost::property_tree::ptree::value_type& itemPair : rowPair.second) {
                    int value = itemPair.second.get_value<int>();
                    v.push_back(value);
                }
            }
            for (size_t i = 0; i < v.size(); i += 2) {
                P point(v[i], v[i+1]);
                pv.push_back(point);
            }
            int identifier_polygon = count++;
            char mychar = 'A';
            // arr1.push_back(*(new Point<dtype>(414, 214)));
            cout << "arr1.push_back(*(new Point<dtype>(" << pv[0].to_string("", " ", " ") << ")));" << endl;
            int identifier_region = tree->insert(new Polygon<dtype>(pv, identifier_polygon));
            // tree->print();
            // json_string = "{\"status\": true, \"identifier_polygon\":" + to_string (identifier_polygon) + "}";
            json_string = "{\"status\": true}";
            stream << json_string;
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //Post search/nearest
    server.resource["^/rtree/nearest$"]["POST"] = [&tree](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "{\"polygons\": [ ";
        vector<dtype> v;
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            ptree pt;
            read_json(request->content, pt);
            for (boost::property_tree::ptree::value_type& rowPair:pt.get_child("polygon")) {
                for (boost::property_tree::ptree::value_type& itemPair : rowPair.second) {
                    int value = itemPair.second.get_value<int>();
                    v.push_back(value);
                }
            }
            int k = pt.get_child("k").get_value<int>();
            P point(v[0], v[1]);

            vector <Polygon<dtype> * > array = tree->nearestSearch(point, k);
            for (size_t i = 0; i < array.size(); i += 1) {
                json_string += to_string(array[i]->get_id()) + ",";
            }
            json_string.pop_back();
            json_string += "]}";
            stream << json_string;
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //Post search/range
    server.resource["^/search/range$"]["POST"] = [&tree](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "{}";
        stream << json_string;
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            ptree pt;
            read_json(request->content, pt);
            int min = pt.get_child("min").get_value<int>();
            int max = pt.get_child("max").get_value<int>();
            cout << min << " " << max << endl;
            //use polygonVector for search
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //options search/range
    server.resource["^/search/range$"]["OPTIONS"] = [](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "{}";
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            json_string = "['status': true]";
            stream << json_string;
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //Option rtree
    server.resource["^/rtree$"]["OPTIONS"] = [](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "";
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            json_string = "['status': true]";
            stream << json_string;
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //Option rtree
    server.resource["^/rtree/nearest$"]["OPTIONS"] = [](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "";
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            json_string = "['status': true]";
            stream << json_string;
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //Delete rtree
    server.resource["^/rtree$"]["DELETE"] = [&tree](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "{}";
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            ptree pt;
            read_json(request->content, pt);
            int id = pt.get_child("id").get_value<int>();
            cout << id << endl;
            //use PolygonVector for delete pg
            json_string = "['status': true]";
            stream << json_string;
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    //delete/reset rtree
    server.resource["^/rtree/reset$"]["DELETE"] = [&tree](
            shared_ptr<HttpServer::Response> response,
            shared_ptr<HttpServer::Request> request
        ) {
        stringstream stream;
        string json_string = "{}";
        SimpleWeb::CaseInsensitiveMultimap header;
        try {
            tree = new RTree<dtype>(2,5);
            json_string = "['status': true]";
            stream << json_string;
            response->write_get(stream,header);
            response->write_get(stream,header);
        } catch (const exception &e) {
            response->write(
                SimpleWeb::StatusCode::client_error_bad_request,
                e.what()
            );
        }
    };

    server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
        // Handle errors here
        // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
    };

    thread server_thread([&server]() {
        // Start server
        server.start();
    });

    // Wait for server to start so that the client can connect
    this_thread::sleep_for(chrono::seconds(1));
    server_thread.join();
}
