# webserver_cpp

Design of 'Basic but Non-Totally-Trivial Webserver': 
    - Server class for setup and listening 
    - Connection class is handling an individual client I/O
    - Response class parses request, grabs a file,and passes it along to 
        connection class

Single-threaded blocking server currently 