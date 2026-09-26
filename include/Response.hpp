#pragma once
#include <string>
#include <map>

// ── HTTP response ─────────────────────────────────────────────────────────────
struct Response {
    int                                status;   // e.g. 200
    std::map<std::string, std::string> headers;  // e.g. "Content-Type": "text/html"
    std::string                        body;

    Response();
    explicit Response(int statusCode);

    // Produce the full HTTP/1.1 wire representation.
    // Always adds: Content-Length, Date, Server, Connection: close.
    std::string serialize() const;
};

// ── Status-code utilities ─────────────────────────────────────────────────────
namespace HttpStatus {
    // Return canonical reason phrase for code (e.g. 404 → "Not Found").
    // Returns "Unknown" for unrecognised codes.
    const char* reason(int code);
}

// ── MIME type lookup ──────────────────────────────────────────────────────────
namespace MimeTypes {
    // Return MIME type string for file extension (with dot, e.g. ".html").
    // Returns "application/octet-stream" for unknown extensions.
    const std::string& lookup(const std::string& ext);
}

// ── Default error page ────────────────────────────────────────────────────────
// Returns a minimal self-contained HTML page for the given status code.
// Used when the config has no error_page directive for that code.
std::string defaultErrorPage(int code);
