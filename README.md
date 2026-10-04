# 🌐 Webserv

## 🧠 Overview

**Webserv** is a custom HTTP/1.1 web server built from scratch in **C++98**, inspired by the behavior of servers such as **Nginx**.

Instead of relying on an existing HTTP framework, the project handles the complete request lifecycle manually — from accepting TCP connections and parsing raw HTTP requests to routing, serving files, processing uploads, executing CGI scripts and generating HTTP responses.

The server is built around a **non-blocking event loop using `poll()`**, allowing multiple clients to be handled without creating a thread for every connection.

This project focuses on:

- TCP sockets
- HTTP/1.1
- Non-blocking I/O
- `poll()` event handling
- Request parsing
- Server routing
- CGI execution
- File uploads
- HTTP response generation
- Configuration parsing
- Process management

---

## 📂 Project Structure

```text
.
├── Makefile
├── conf/
│   └── default.conf
│
├── include/
│   ├── Cgi.hpp
│   ├── Client.hpp
│   ├── Config.hpp
│   ├── Handlers.hpp
│   ├── IPoll.hpp
│   ├── Listener.hpp
│   ├── PollLoop.hpp
│   ├── Request.hpp
│   └── Response.hpp
│
├── src/
│   ├── network/
│   │   ├── main.cpp
│   │   ├── Listener.cpp
│   │   ├── Client.cpp
│   │   └── PollLoop.cpp
│   │
│   ├── http/
│   │   ├── Request.cpp
│   │   └── Response.cpp
│   │
│   └── app/
│       ├── Config.cpp
│       ├── Handlers.cpp
│       └── Cgi.cpp
│
└── www/
    └── html/
        └── index.html
```

The server is divided into clear layers for **networking, HTTP protocol handling and application-level routing**.

---

## 🚀 Compilation

Compile the server:

```bash
make
```

This generates:

```text
webserv
```

Run it with a configuration file:

```bash
./webserv conf/default.conf
```

Then open:

```text
http://localhost:8080
```

Clean object files:

```bash
make clean
```

Remove everything:

```bash
make fclean
```

Recompile:

```bash
make re
```

Compilation uses:

```text
-Wall -Wextra -Werror -std=c++98
```

---

## ⚙️ How It Works

A request travels through several layers before the client receives a response:

```text
Browser / Client
       ↓
    TCP Socket
       ↓
     poll()
       ↓
HTTP Request Parser
       ↓
     Router
       ↓
 Request Handler
       ↓
Static File / Upload / CGI
       ↓
HTTP Response
       ↓
     Client
```

For example:

```http
GET /index.html HTTP/1.1
Host: localhost:8080
```

Webserv must accept the connection, parse the raw bytes, find the correct route, locate the requested resource, create a valid HTTP response and send it back to the client.

---

## ⚡ Non-Blocking Server

The core of Webserv is its event-driven networking model.

Instead of blocking while waiting for one client, sockets are placed into a single:

```cpp
poll()
```

event loop.

The server monitors connections for events such as:

```text
POLLIN  → Data is ready to read
POLLOUT → Data is ready to send
POLLERR → Socket error
POLLHUP → Client disconnected
```

This allows Webserv to manage multiple connections while remaining inside a single event-driven architecture.

---

## 📡 HTTP Request Parsing

Incoming network data arrives as raw bytes and may not contain the entire HTTP request at once.

Webserv therefore uses an **incremental HTTP parser** with states for:

```text
Request Line
     ↓
Headers
     ↓
Body
     ↓
Complete Request
```

Example:

```http
POST /uploads HTTP/1.1
Host: localhost
Content-Length: 11

Hello World
```

The parser extracts:

- HTTP method
- Request path
- Query string
- HTTP version
- Headers
- Request body

It also handles:

```text
Content-Length
Transfer-Encoding: chunked
URL decoding
```

---

## 📤 HTTP Responses

Webserv builds HTTP/1.1 responses manually.

Example:

```http
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 125
Server: webserv/1.0

<html>...</html>
```

Responses include the appropriate:

- Status code
- Reason phrase
- Headers
- MIME type
- Content length
- Response body

Supported status handling includes codes such as:

```text
200 OK
201 Created
204 No Content
301 Moved Permanently
400 Bad Request
403 Forbidden
404 Not Found
405 Method Not Allowed
413 Request Entity Too Large
500 Internal Server Error
502 Bad Gateway
504 Gateway Timeout
```

---

## 🛣 Routing & Configuration

Webserv uses its own configuration format inspired by Nginx.

Example:

```nginx
server {
    listen      8080;
    host        0.0.0.0;
    server_name localhost;

    client_max_body_size 10M;

    location / {
        methods     GET;
        root        ./www/html;
        index       index.html;
        autoindex   off;
    }

    location /uploads {
        methods         GET POST DELETE;
        root            ./www/uploads;
        upload_store    ./www/uploads;
        autoindex       on;
    }
}
```

Locations can define:

- Allowed HTTP methods
- Root directories
- Index files
- Redirects
- Autoindex
- Upload directories
- Maximum body size
- CGI configuration

The server selects the most appropriate location based on the requested URL.

---

## 📦 HTTP Methods

Webserv handles the main methods required by the project.

### GET

```http
GET /index.html HTTP/1.1
```

Used to retrieve static resources and files.

### POST

```http
POST /uploads HTTP/1.1
```

Used to send data and upload files to the server.

### DELETE

```http
DELETE /uploads/file.txt HTTP/1.1
```

Used to remove resources from the server.

---

## 📁 Static Files & Autoindex

Webserv can serve files directly from configured directories.

Examples include:

```text
.html
.css
.js
.json
.png
.jpg
.svg
.txt
.pdf
```

The appropriate `Content-Type` is selected automatically using the file extension.

Directories can also enable:

```nginx
autoindex on;
```

allowing Webserv to dynamically generate an HTML directory listing.

---

## 📤 File Uploads

POST requests can use:

```text
multipart/form-data
```

to upload files.

Webserv parses the multipart boundary, extracts the uploaded file and stores it inside the configured:

```nginx
upload_store
```

directory.

This means the server is doing the multipart parsing itself rather than relying on an external web framework.

---

## ⚙️ CGI

Webserv supports **CGI — Common Gateway Interface**, allowing requests to execute external scripts.

Example configuration:

```nginx
location /cgi-bin {
    methods         GET POST;
    root            ./www/cgi-bin;
    cgi_extension   .py;
    cgi_path        /usr/bin/python3;
}
```

A CGI request involves:

```text
HTTP Request
     ↓
Create Pipes
     ↓
fork()
     ↓
Prepare CGI Environment
     ↓
execve()
     ↓
Execute Script
     ↓
Read CGI Output
     ↓
HTTP Response
```

The server communicates with the CGI process through pipes while integrating it into the same event-driven architecture.

---

## 🧪 Testing

The server can be tested using:

```bash
curl http://localhost:8080/
```

Headers can be inspected with:

```bash
curl -i http://localhost:8080/
```

POST request:

```bash
curl -X POST http://localhost:8080/uploads
```

DELETE request:

```bash
curl -X DELETE http://localhost:8080/uploads/file.txt
```

Concurrent connections and error cases should also be tested to verify that the server remains stable under multiple requests.

---

## 🎯 Objectives

Webserv provides hands-on experience with:

- Understanding how HTTP actually works
- Building TCP networking from scratch
- Managing multiple clients with `poll()`
- Working with non-blocking file descriptors
- Parsing network protocols
- Designing event-driven software
- Creating configurable routing systems
- Handling file uploads
- Executing external processes through CGI
- Managing pipes, sockets and child processes
- Understanding what happens behind a web framework

---

## 🏆 What Makes Webserv Interesting

A browser normally hides almost everything happening behind a simple URL.

With Webserv:

```text
localhost:8080
      ↓
TCP Connection
      ↓
Socket
      ↓
poll()
      ↓
Raw HTTP Bytes
      ↓
HTTP Parser
      ↓
Router
      ↓
File / Upload / CGI
      ↓
HTTP Response
      ↓
Browser
```

No Apache.

No Nginx.

No web framework.

Just **C++98, sockets, HTTP, processes and an event loop**.
