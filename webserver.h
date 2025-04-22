#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <cstring>
#include <unistd.h> // for read(), write(), close()
#include <sys/socket.h> // for send() and recv() and accept()
#include <sys/stat.h> // gets file information with struct stat 
#include <netinet/in.h> // definitions for IP protocol family 
#include <arpa/inet.h> // for sockaddr_in
#include <sstream>  // for stringstream 
#include <fcntl.h>    // for open(), O_RDONLY, O_WRONLY, O_CREAT, etc.

/*
Design of 'Basic but Non-Totally-Trivial Webserver': 
    - Server class for setup and listening 
    - Connection class is handling an individual client I/O
    - Response class parses request, grabs a file,and passes it along to 
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
            void handleConnection();

        private:
            int m_client_socket; 
    };

    class Response {
        public:
            Response(int client_socket);
            ~Response();

            void handleRequest();

            bool parseHttpRequest(  const std::string& request, 
                                    std::string& requestedFile );

            void sendTextHttpResponse(  const std::string& status,         
                                        const char* body,
                                        size_t bodyLength );

            void sendFile(  const std::string& basePath, 
                            const std::string& requestedFile );
            
            // Todo later:
            // Helper function to check for safe file paths
            // i.e. 'bool isPathSafe' 

        private:
            int m_client_socket; 
    };

}


#endif 
















/*
#include <fcntl.h>       // for open()
#include <unistd.h>      // for read(), write(), close()
#include <sys/socket.h>  // for send()
#include <sys/stat.h>    // for fstat()
#include <string>
#include <cstring>       // for strerror
#include <iostream>
#include <limits.h>      // for PATH_MAX
#include <stdlib.h>      // for realpath
#include <sstream>       // for stringstream

// Helper function to check for safe paths
bool isPathSafeStrict(const std::string& basePath, const std::string& requestedFile) {
    std::string fullPath = basePath + requestedFile;

    char resolvedPath[PATH_MAX];
    if (realpath(fullPath.c_str(), resolvedPath) == nullptr) {
        return false;
    }

    return std::string(resolvedPath).rfind(basePath, 0) == 0;
}

// Helper function to send HTTP response headers and body
void sendHttpResponse(int clientSocket, const std::string& status, const std::string& contentType, const char* body, size_t bodyLength) {
    std::string header = 
        "HTTP/1.1 " + status + "\r\n" +
        "Content-Type: " + contentType + "\r\n" +
        "Content-Length: " + std::to_string(bodyLength) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n";

    send(clientSocket, header.c_str(), header.size(), 0);
    send(clientSocket, body, bodyLength, 0);
}

// Function to serve the requested file over the socket
void sendHttpFile(int clientSocket, const std::string& basePath, const std::string& requestedFile) {
    if (!isPathSafeStrict(basePath, requestedFile)) {
        const char* errorMsg = "Invalid file path.\n";
        sendHttpResponse(clientSocket, "400 Bad Request", "text/plain", errorMsg, strlen(errorMsg));
        return;
    }

    std::string fullPath = basePath + requestedFile;
    int fileFd = open(fullPath.c_str(), O_RDONLY);
    if (fileFd < 0) {
        const char* notFoundMsg = "File not found.\n";
        sendHttpResponse(clientSocket, "404 Not Found", "text/plain", notFoundMsg, strlen(notFoundMsg));
        return;
    }

    struct stat stat_buf;
    if (fstat(fileFd, &stat_buf) < 0) {
        close(fileFd);
        const char* errorMsg = "Server error.\n";
        sendHttpResponse(clientSocket, "500 Internal Server Error", "text/plain", errorMsg, strlen(errorMsg));
        return;
    }

    std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Length: " + std::to_string(stat_buf.st_size) + "\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(clientSocket, header.c_str(), header.size(), 0);

    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(fileFd, buffer, sizeof(buffer))) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    close(fileFd);
}

// Function to parse the HTTP request and extract the file path
bool parseHttpRequest(const std::string& request, std::string& requestedFile) {
    std::istringstream requestStream(request);
    std::string method, path, version;

    // Parse the request line: METHOD /path HTTP/version
    requestStream >> method >> path >> version;

    // Only handle GET requests in this minimal server
    if (method != "GET") {
        return false;
    }

    // Clean up the path (e.g., decode URL encoding if needed)
    if (path == "/") {
        requestedFile = "/index.html"; // Default file if root is requested
    } else {
        requestedFile = path;
    }

    return true;
}

void handleRequest(int clientSocket) {
    // Read the HTTP request from the socket
    char buffer[4096];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        return; // No data or error
    }

    std::string request(buffer, bytesRead);
    std::string requestedFile;

    // Parse the HTTP request to extract the file path
    if (!parseHttpRequest(request, requestedFile)) {
        const char* errorMsg = "Invalid HTTP request.\n";
        sendHttpResponse(clientSocket, "400 Bad Request", "text/plain", errorMsg, strlen(errorMsg));
        return;
    }

    // Serve the requested file (from a base directory)
    std::string basePath = "/var/www"; // Base directory where files are stored
    sendHttpFile(clientSocket, basePath, requestedFile);
}





*/