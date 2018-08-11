#include "client_http.hpp"
#include "server_http.hpp"
#include <iostream>
#include <typeinfo>
#include <string>
#include <utility>
//Models for rtree
// #include "models/Polygon.cpp"
// #include "models/Region.cpp"
#include "models/RTree.h"
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
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;


int main() {
    // HTTP-server at port 8080 using 1 thread
    // Unless you do more heavy non-threaded processing in the resources,
    // 1 thread is usually faster than several threads
    HttpServer server;
    server.config.port = 8090;
    // vector<Polygon> polygonVector;
    // vector<Region> regionVector;
    RTree<dtype> * tree = new RTree<dtype>(2,5);

    // vector<Point> rp;
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
            // cout << pv[0].to_string() << endl;
            int identifier_polygon = count++;
            int identifier_region = tree->insert(new Polygon<dtype>(pv, identifier_polygon));
            // tree->print();
            json_string = "{\"status\": true, \"identifier_polygon\":" + to_string(identifier_polygon) + ", \"identifier_region\":" + to_string(identifier_region) + "}";
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
            // polygonVector.clear();
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

    // Default GET-example. If no other matches, this anonymous function will be called.
    // Will respond with content in the web/-directory, and its subdirectories.
    // Default file: index.html
    // Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
    server.default_resource["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        try {
        auto web_root_path = boost::filesystem::canonical("web");
        auto path = boost::filesystem::canonical(web_root_path / request->path);
        // Check if path is within web_root_path
        if(distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
            !equal(web_root_path.begin(), web_root_path.end(), path.begin()))
            throw invalid_argument("path must be within root path");
        if(boost::filesystem::is_directory(path))
            path /= "index.html";

        SimpleWeb::CaseInsensitiveMultimap header;

        // Uncomment the following line to enable Cache-Control
        // header.emplace("Cache-Control", "max-age=86400");

    #ifdef HAVE_OPENSSL
    //    Uncomment the following lines to enable ETag
    //    {
    //      ifstream ifs(path.string(), ifstream::in | ios::binary);
    //      if(ifs) {
    //        auto hash = SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
    //        header.emplace("ETag", "\"" + hash + "\"");
    //        auto it = request->header.find("If-None-Match");
    //        if(it != request->header.end()) {
    //          if(!it->second.empty() && it->second.compare(1, hash.size(), hash) == 0) {
    //            response->write(SimpleWeb::StatusCode::redirection_not_modified, header);
    //            return;
    //          }
    //        }
    //      }
    //      else
    //        throw invalid_argument("could not read file");
    //    }
    #endif

        auto ifs = make_shared<ifstream>();
        ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

        if(*ifs) {
            auto length = ifs->tellg();
            ifs->seekg(0, ios::beg);

            header.emplace("Content-Length", to_string(length));
            response->write(header);

            // Trick to define a recursive function within this scope (for example purposes)
            class FileServer {
            public:
            static void read_and_send(const shared_ptr<HttpServer::Response> &response, const shared_ptr<ifstream> &ifs) {
                // Read and send 128 KB at a time
                static vector<char> buffer(131072); // Safe when server is running on one thread
                streamsize read_length;
                if((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
                response->write(&buffer[0], read_length);
                if(read_length == static_cast<streamsize>(buffer.size())) {
                    response->send([response, ifs](const SimpleWeb::error_code &ec) {
                    if(!ec)
                        read_and_send(response, ifs);
                    else
                        cerr << "Connection interrupted" << endl;
                    });
                }
                }
            }
            };
            FileServer::read_and_send(response, ifs);
        }
        else
            throw invalid_argument("could not read file");
        }
        catch(const exception &e) {
        response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
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

    // Client examples
    HttpClient client("localhost:8090");

    string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

    // Synchronous request examples
    try {
        /*auto r1 = client.request("GET", "/match/123");
        cout << r1->content.rdbuf() << endl; // Alternatively, use the convenience function r1->content.string()

        auto r2 = client.request("POST", "/string", json_string);
        cout << r2->content.rdbuf() << endl;*/
    }
    catch(const SimpleWeb::system_error &e) {
        cerr << "Client request error: " << e.what() << endl;
    }

    // Asynchronous request example
    client.request("POST", "/json", json_string, [](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
        if(!ec)
        cout << response->content.rdbuf() << endl;
    });
    client.io_service->run();

    server_thread.join();
}
