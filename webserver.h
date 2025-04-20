#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <cstring>
#include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
Design of 'Basic but Non-Trivial Webserver': 
    - Separate socket management (Server) for setup and listening 
    - Connection class is handling an individual client I/O
    - Response class builds the HTML response, and passes it along to 
        connection class

Single-threaded blocking server currently 
*/

namespace WebServer {

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

    class Connection {
        public:
            Connection(int client_socket);
            ~Connection(); 

            void connectionTest();

            void sendHttpResponse(  int clientSocket, 
                                    const std::string& status, 
                                    const std::string& contentType, 
                                    const char* body,
                                    size_t bodyLength );

            void sendFile(  int clientSocket, 
                            const std::string& basePath, 
                            const std::string& requestedFile );

            // Todo later:
            // Helper function to check for safe file paths
            // i.e. 'bool isPathSave' 

        private:
            int m_client_socket; 
    };

    class Response {
        public:

        private:

    };

}


#endif 