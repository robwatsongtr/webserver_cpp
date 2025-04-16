#include "webserver.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ---- Server constructor and destructor implementations -----------

WebServer::Server::Server(std::string ip_address, int port) : 
                                    m_ip_address(ip_address), 
                                    m_port(port), 
                                    m_server_socket(),
                                    m_server_addr(), // struct sockaddr_in
                                    m_client_addr() // struct
{
    // set up server address 
    memset(&m_server_addr, 0, sizeof(m_server_addr)); // zero out struct
    m_server_addr.sin_family = AF_INET; // internet comms, not IPC or other
    m_server_addr.sin_port = htons(m_port); // Convert to network byte order
    m_server_addr.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces
}

WebServer::Server::~Server() {
    close(m_server_socket);
}

// ------ Server Method implementations --------------------------------

int WebServer::Server::startServer() {
    // create socket of domain internet, type sockstream, protocol 0
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (m_server_socket < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    // bind socket to address. Call bind and immediately check for error state 
    if (bind(m_server_socket, (struct sockaddr *) &m_server_addr, 
        sizeof(m_server_addr)) < 0) 
    {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    // listen on socket
    if (listen(m_server_socket, SOMAXCONN) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        return 1;
    }

    return 0;
}

void WebServer::Server::acceptConnections() {
    std::cout << "Server listening on port " << ntohs(m_server_addr.sin_port) 
        << std::endl;

        while (true) {
            socklen_t client_len = sizeof(m_client_addr);   
            client_socket = accept(m_server_socket, (struct sockaddr *) 
                &m_client_addr, &client_len);

            if (client_socket < 0) {
                std::cerr << "Error accepting connection" << std::endl;
                continue;
            }

            // Declares a char array big enough to hold a str or IP$ address
            char client_ip[INET_ADDRSTRLEN];
            // This converts the raw binary IP address into a human-readable string.
            inet_ntop(AF_INET, &(m_client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(m_client_addr.sin_port);

            std::cout << "Accepted connection from " << client_ip << ":" << client_port 
                << std::endl;

            // pass off connection to Connection class 
            WebServer::Connection new_conn(client_socket);
            new_conn.handleTest();
        }
}


// ---- Connection constructor and destructor implementations ––------------- 

WebServer::Connection::Connection(int client_socket) 
    : m_client_socket(client_socket) {}

WebServer::Connection::~Connection() {
    close(m_client_socket);
}

// ------ Connection Method implementations ----------------------------

void WebServer::Connection::handleTest() {
    const char* response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 14\r\n"
        "\r\n"
        "Hello, world!\n";
    
    send(m_client_socket, response, strlen(response), MSG_NOSIGNAL);
}




