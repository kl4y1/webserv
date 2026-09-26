#include "Request.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>

// ── CaseInsensitiveLess ───────────────────────────────────────────────────────
bool CaseInsensitiveLess::operator()(const std::string& a,
                                     const std::string& b) const {
    // Compare a and b character by character using tolower()
    // Return true if a < b in case-insensitive order
    (void)a; (void)b;
    return false;
}

// ── Request ───────────────────────────────────────────────────────────────────
Request::Request() : parseState(PS_REQUEST_LINE), statusCode(0) {}

// ── RequestParser ─────────────────────────────────────────────────────────────
RequestParser::RequestParser() : bodyRemain_(0) {}

void RequestParser::feed(const char* data, size_t len) {
    // 1. Append data to buf_
    // 2. Enter a loop; switch on req_.parseState:
    //    PS_REQUEST_LINE → parseRequestLine(); break if buf_ doesn't yet contain "\r\n"
    //    PS_HEADERS      → parseHeaders();     break if headers block not yet complete
    //    PS_BODY_LENGTH  → parseBodyLength();  break if not enough bytes yet
    //    PS_BODY_CHUNKED → parseBodyChunked(); break when chunk stream ends
    //    PS_DONE / PS_ERROR → stop; leftover bytes in buf_ belong to next request
    (void)data; (void)len;
}

bool RequestParser::done()  const { return req_.parseState == PS_DONE;  }
bool RequestParser::error() const { return req_.parseState == PS_ERROR; }

const Request& RequestParser::request() const { return req_; }
Request&       RequestParser::request()       { return req_; }

void RequestParser::reset() {
    // Re-initialise req_ to a fresh Request()
    // Clear buf_, bodyRemain_, chunkBuf_
    // Do NOT clear buf_ — leftover bytes may be a pipelined second request;
    // keep them and start parsing the next request immediately.
}

void RequestParser::parseRequestLine() {
    // Find "\r\n" in buf_; if not found, return (need more data)
    // Extract: method, raw target, version
    // Validate method is one of GET/POST/DELETE (else set PS_ERROR + 501)
    // Validate version starts with "HTTP/" (else PS_ERROR + 400)
    // Split target at '?' into req_.path and req_.query
    // URL-decode both req_.path and req_.query
    // Erase the consumed line from buf_
    // Advance to PS_HEADERS
}

void RequestParser::parseHeaders() {
    // Read lines from buf_ until a blank line "\r\n\r\n" (or "\n\n")
    // For each non-blank line: split at first ':'; trim whitespace; store in req_.headers
    // After blank line:
    //   if Transfer-Encoding == "chunked" → PS_BODY_CHUNKED
    //   else if Content-Length present → bodyRemain_ = value; PS_BODY_LENGTH
    //   else → PS_DONE (no body)
    // Reject Content-Length > client_max_body_size with PS_ERROR + 413
    //   (client_max_body_size is not accessible here; Handlers.cpp will do a
    //    second check; the parser only rejects obviously huge Content-Length if needed)
}

void RequestParser::parseBodyLength() {
    // If buf_.size() < bodyRemain_: return (need more data)
    // req_.body = buf_.substr(0, bodyRemain_)
    // Erase bodyRemain_ bytes from buf_
    // Advance to PS_DONE
}

void RequestParser::parseBodyChunked() {
    // Chunked format: "<hex-size>\r\n<data>\r\n" ... "0\r\n\r\n"
    // Loop:
    //   1. Read hex size from buf_; if no "\r\n" yet, return
    //   2. chunkSize = strtol(hexStr, ...)
    //   3. if chunkSize == 0 → PS_DONE; return
    //   4. if buf_ doesn't have chunkSize + 2 bytes after size line, return
    //   5. Append chunk bytes to req_.body
    //   6. Erase size line + chunk bytes + "\r\n" from buf_
}

std::string RequestParser::urlDecode(const std::string& src) {
    // Walk src char by char:
    //   '+' → ' '
    //   '%' followed by two hex digits → the corresponding byte
    //   anything else → copy as-is
    // Return the decoded string
    (void)src;
    return "";
}
