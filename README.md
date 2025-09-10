# Basic but Non-Totally-Trivial Webserver

- **Server class**  
  Responsible for setup and listening for incoming connections.

- **Connection class**  
  Handles an individual client’s I/O.

- **Response class**  
  Parses the request, grabs a file, and passes it along to the connection class.

---

**Current state:**  
Single-threaded, blocking server.
