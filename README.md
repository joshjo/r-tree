R Tree
========

### Compile and run

Compile with a C++11 compliant compiler:
```sh
mkdir build
cd build
cmake ..
make
cd ..
```

### Usage

See http_examples.cpp or https_examples.cpp for example usage.

See particularly the JSON-POST (using Boost.PropertyTree) and the GET /match/[number] examples, which are most relevant.

### Dependencies

* Boost.Asio or standalone Asio
* Boost is required to compile the examples
* For HTTPS: OpenSSL libraries

#### HTTP

Run the server and client examples: `./build/http_examples`

Direct your favorite browser to for instance http://localhost:8080/
