#include "webserver.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

WebServer::Connection::Connection(int client_socket) : client_socket(client_socket) {}

WebServer::Connection::~Connection() {
    // if check because an invalid socket returns -1 in POSIX
    if (client_socket >= 0) {
        close(client_socket);
    } 
}