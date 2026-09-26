#include "Response.hpp"
#include <ctime>
#include <sstream>

Response::Response() : status(200) {}

Response::Response(int statusCode) : status(statusCode) {}

std::string Response::serialize() const {
    // Build the HTTP/1.1 response wire format:
    // 1. Status line: "HTTP/1.1 <status> <reason>\r\n"
    // 2. Always-present headers:
    //    - Date: <RFC 1123 formatted time from gmtime()>
    //    - Server: webserv/1.0
    //    - Connection: close
    //    - Content-Length: <body.size()>
    // 3. All entries from headers map
    // 4. "\r\n"   (blank line)
    // 5. body
    // Use std::ostringstream to build; return as std::string
    return "";
}

const char* HttpStatus::reason(int code) {
    switch(code){
    case 200 : return "OK";
    case 201 : return "Created";
    case 204 : return "No Content";
    case 301 : return "Moved Permanently";
    case 400 : return "Bad Request";
    case 403 : return "Forbidden";
    case 404 : return "Not Found";
    case 405 : return "Method Not Allowed";
    case 408 : return "Request Timeout";
    case 411 : return "Length Required";
    case 413 : return "Request Entity Too Large";
    case 500 : return "Internal Server Error";
    case 501 : return "Not Implemented";
    case 502 : return "Bad Gateway";
    case 504 : return "Gateway Timeout";
    default : return "Unknown";}
}

const std::string& MimeTypes::lookup(const std::string& ext) {
    static std::map<std::string, std::string> types;
    static const std::string fallback = "application/octet-stream";

    if (types.empty()) {
        types[".html"] = "text/html";
        types[".htm"] = "text/html";
        types[".css"] = "text/css";
        types[".js"] = "application/javascript";
        types[".json"] = "application/json";
        types[".png"] = "image/png";
        types[".jpg"] = "image/jpeg";
        types[".jpeg"] = "image/jpeg";
        types[".gif"] = "image/gif";
        types[".svg"] = "image/svg+xml";
        types[".ico"] = "image/x-icon";
        types[".txt"] = "text/plain";
        types[".pdf"] = "application/pdf";
    }

    std::map<std::string, std::string>::const_iterator it = types.find(ext);
    if (it != types.end())
        return it->second;
    return fallback;
}

std::string defaultErrorPage(int code) {
    std::ostringstream out;
    const char* reason = HttpStatus::reason(code);

    out << "<html><head><title>" << code << " " << reason << "</title></head><body><h1>" << code << " " << reason << "</h1><hr><p>webserv</p></body></html>";

    return out.str();
}
