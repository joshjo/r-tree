R Tree
========
#### Requirements
```sh
Install brew
copy and paste the next command in terminal 
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

#### Dependencies
```sh
Install libboost-all-dev
command for install libboost-all-dev
brew install boost
```
```sh
Install g++
command for install g++ 
brew install g++ 
```
```sh
Install cmake
command for install cmake
brew install cmake 
```
```sh
Install make
command for install make
brew install make 
```

#### Compile and run server

```sh
mkdir build
cd build
cmake ..
make
cd ..
Run the server: `./build/run_server`
```

#### Compile and run Client

....

Direct your favorite browser to for instance http://localhost:8080/
