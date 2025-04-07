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
            ~Connection(); // Ensure client socket is closed when connection object goes out of scope

        private:
            int m_client_socket; // take ownership of request 
            std::string buildResponse();
            void sendResponse();    
    };

    //  
    class Server {
        public:
            Server(std::string ip_address, int port);
            ~Server(); // close listening socket on server shutdown

            void startListen();
            
        private:
            int m_port;
            int m_server_socket;
            std::string m_ip_address;

            void acceptConnections();
            void closeServer();
    };

}


#endif 