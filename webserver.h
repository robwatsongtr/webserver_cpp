#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*

Design of webserver: 
    Separate socket management (Server) for setup and listening 
    Connection class is handling an individual client. 

Single-threaded blocking server currently (4/7/25)

*/

namespace WebServer {

    class Connection {
        public:
            Connection(int client_socket);
            ~Connection(); 

        private:
            int m_client_socket; 
            std::string buildResponse();
            void sendResponse();    
    };

    class Server {
        public:
            Server(std::string ip_address, int port);
            ~Server(); 
            void acceptConnections();
            int startServer();

        private:
            std::string m_ip_address;
            int m_port;
            int m_server_socket;
            int client_socket; 
            struct sockaddr_in m_server_addr; 
            struct sockaddr_in m_client_addr; // passed to Connection
    };

}


#endif 