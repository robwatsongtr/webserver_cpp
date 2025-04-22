#include "webserver.h"

// ----------------- Server Class implementations ---------------------

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
    m_server_addr.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces NOT SAFE
}

WebServer::Server::~Server() {
    close(m_server_socket);
}

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

        // Declare a char array big enough to hold a str or ip address
        // Then convert the raw binary IP address into a human-readable string.
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(m_client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        int client_port = ntohs(m_client_addr.sin_port);

        std::cout << "Accepted connection from " << client_ip << ":" << client_port 
            << std::endl;

        // pass off connection to Connection class 
        WebServer::Connection newConn(client_socket);
        newConn.handleConnection();
    }
}


// ---------------------- Connection Class implementations --------––------------- 

WebServer::Connection::Connection(int client_socket) 
    : m_client_socket(client_socket) {}

WebServer::Connection::~Connection() {
    // good quality RIAA here. 
    close(m_client_socket);
}

void WebServer::Connection::connectionTest() {
    const char* response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 14\r\n"
        "\r\n"
        "Hello, world!\n";
    
    send(m_client_socket, response, strlen(response), MSG_NOSIGNAL);
}

void WebServer::Connection::handleConnection() {
    // simply hand off to a response object. TODO for future: Error handling, logging 
    WebServer::Response response(m_client_socket);
    response.handleRequest();
}


// ---------------------- Response Class implementations --------––------------- 

WebServer::Response::Response(int client_socket) 
    : m_client_socket(client_socket) {}

WebServer::Response::~Response() {}

void WebServer::Response::handleRequest() {
    // 'receive' http request from socket with recv(). POSIX likes ssize_t bc its signed
    char buffer[4096];
    ssize_t bytesRead = recv(m_client_socket, buffer, sizeof(buffer), 0);

    if (bytesRead < 0) {
        std::cout << "recv failed" << std::endl;
    } 

    // create string object containing data, and its size so all the data gets in 
    std::string incomingRequest(buffer, bytesRead);

    // init a string object to hold the requested file. Gets passed by ref to parser
    std::string requestedFile; 

    bool isValidRequest = parseHttpRequest(incomingRequest, requestedFile);
    if (!isValidRequest) {
        const char* msg = "Invalid HTTP request.\n";
        sendTextHttpResponse("400 Bad Request", msg, strlen(msg));
        return;
    }

    std::string basePath = "/users/robertwatson/Code/webserver_cpp/public";
    sendFile(basePath, requestedFile);

}

bool WebServer::Response::parseHttpRequest( const std::string& request, 
                                            std::string& requestedFile ) 
{
    // parse request with istringstream which conveniently works on whitespace.
    std::istringstream reqStream(request);
    std::string method, path, version; 
    reqStream >> method >> path >> version;

    if (method != "GET") { return false; } // Only GET for now. 
    requestedFile = (path == "/") ? "/index.html" : path; 

    return true; 
}

void WebServer::Response::sendTextHttpResponse( const std::string& status, 
                                                const char* body,
                                                size_t bodyLength )  
{
   std::string header = 
        "HTTP/1.1 " + status + "\r\n" +
        "Content-Type: " + "text/plain" + "\r\n" +
        "Content-Length: " + std::to_string(bodyLength) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n";

    // send header, then send body 
    send(m_client_socket, header.c_str(), header.size(), 0);
    send(m_client_socket, body, bodyLength, 0);                                   
}

void WebServer::Response::sendFile( const std::string& basePath, 
                                    const std::string& requestedFile )
{
    std::string fullPath = basePath + requestedFile;

    // open the file or it doesn't exist. fd = file descriptor, OS process unique identifier
    int file_fd = open(fullPath.c_str(), O_RDONLY); // 
    if (file_fd < 0) {
        const char* msg = "File not found.\n";
        sendTextHttpResponse("404 Not Found",  msg, strlen(msg));
        return;
    }

    // general check about the validity of the file itself using struct stat 
    // also gets other info like size of file in bytes, permissions, last modified, etc. 
    struct stat fileStat;
    if (fstat(file_fd, &fileStat) < 0) {
        close(file_fd);
        const char* msg = "Server error.\n";
        sendTextHttpResponse("500 Internal Server Error", msg, strlen(msg));
        return;
    }

    // send header. ** hardcoding content type to text/html for now 
    std::string header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(fileStat.st_size) + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";

    send(m_client_socket, header.c_str(), header.size(), 0);

    // send file
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0) {
        send(m_client_socket, buffer, bytesRead, 0);
    }

    close(file_fd);
}

