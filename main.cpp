#include "webserver.h"
#include <iostream>
#include <string>
// #include <cassert>

// g++ main.cpp webserver.cpp -o serve_files

int main() {

    WebServer::Server new_server("127.0.0.1", 8080);

    if (new_server.startServer() == 0) {
        new_server.acceptConnections();
    }

    return 0;
}