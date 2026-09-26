#pragma once
#include <string>
#include <map>

// ── Case-insensitive comparator for header map ───────────────────────────────
struct CaseInsensitiveLess {
    bool operator()(const std::string& a, const std::string& b) const;
};

// ── Parser state machine states ──────────────────────────────────────────────
enum ParseState {
    PS_REQUEST_LINE,   // reading "METHOD /path HTTP/1.x\r\n"
    PS_HEADERS,        // reading header lines
    PS_BODY_LENGTH,    // reading fixed-length body
    PS_BODY_CHUNKED,   // reading chunked body
    PS_DONE,           // complete request available
    PS_ERROR           // bad syntax; statusCode holds the HTTP error
};

// ── Parsed HTTP request ──────────────────────────────────────────────────────
struct Request {
    std::string method;     // "GET", "POST", "DELETE"
    std::string target;     // raw request-target e.g. "/foo?bar=1"
    std::string path;       // URL-decoded path e.g. "/foo"
    std::string query;      // query string e.g. "bar=1"
    std::string version;    // "HTTP/1.1"
    std::map<std::string, std::string, CaseInsensitiveLess> headers;
    std::string body;       // fully assembled, un-chunked body
    ParseState  parseState;
    int         statusCode; // set when parseState == PS_ERROR

    Request();
};

// ── Incremental parser ───────────────────────────────────────────────────────
class RequestParser {
public:
    RequestParser();

    // Feed raw bytes from a recv() call.
    // Updates internal state; may transition to PS_DONE or PS_ERROR.
    void feed(const char* data, size_t len);

    bool done()  const; // true when parseState == PS_DONE
    bool error() const; // true when parseState == PS_ERROR

    // Access the request being assembled
    const Request& request() const;
    Request&       request();

    // Reset to initial state for the next request on the same connection
    void reset();

private:
    Request     req_;
    std::string buf_;        // unparsed bytes accumulated across feed() calls
    size_t      bodyRemain_; // bytes left for PS_BODY_LENGTH
    std::string chunkBuf_;   // partial chunk data for PS_BODY_CHUNKED

    void parseRequestLine();
    void parseHeaders();
    void parseBodyLength();
    void parseBodyChunked();

    // URL-decode %XX sequences in src; result written into req_.path / req_.query
    static std::string urlDecode(const std::string& src);
};
