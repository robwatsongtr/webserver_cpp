#include "webserver.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ---- Server constructor and destructor implementations

WebServer::Server::Server(std::string ip_address, int port) : 
    m_ip_address(ip_address), m_port(port), m_server_socket(-1) {}

WebServer::Server::~Server() {
    if (m_server_socket >= 0) {
        close(m_server_socket);
    } 
}

// ---- Connection constructor and destructor implementations  

WebServer::Connection::Connection(int client_socket) : 
    m_client_socket(client_socket) {}

WebServer::Connection::~Connection() {
    // if check because an invalid socket returns -1 in POSIX
    if (m_client_socket >= 0) {
        close(m_client_socket);
    } 
}

// ------ Server Method implementations 






// ------ Connection Method implementations 