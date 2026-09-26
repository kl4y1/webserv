#pragma once
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "IPoll.hpp"

class Client;

// ── Main dispatch ─────────────────────────────────────────────────────────────
// Called by Client::onReadable() once the parser signals done().
// Selects the matching ServerConfig, resolves the location, enforces method
// rules, and delegates to the right sub-handler.
// For CGI: creates a CgiSession, registers pipes on loop, returns an empty
// "pending" Response (response arrives asynchronously via CgiSession).
Response handle(const Request&      req,
                const ServerConfig& srv,
                IPoll&              loop,
                Client*             client);

// ── Route resolver ────────────────────────────────────────────────────────────
namespace RouteResolver {
    // Longest-prefix match of req.path against srv.locations.
    // Returns a pointer into srv.locations; never returns NULL (falls back
    // to the root location "/" which must exist).
    const LocationConfig* resolve(const std::string&  path,
                                  const ServerConfig& srv);

    // Convert request path to a filesystem path using location root + prefix.
    // Rejects any ".." segment with a 403 (throws std::runtime_error with
    // the status code encoded).
    std::string toFilesystemPath(const std::string&    reqPath,
                                 const LocationConfig& loc);
}

// ── Static file / directory handler ──────────────────────────────────────────
namespace StaticHandler {
    // stat() the path:
    //   regular file → read whole file, set Content-Type from extension → 200
    //   directory     → try index file; else autoindex HTML; else 403
    //   not found     → 404
    Response handle(const Request&        req,
                    const LocationConfig& loc,
                    const std::string&    fsPath,
                    const ServerConfig&   srv);

    // Build an HTML directory listing for dirPath using opendir/readdir.
    std::string autoindexPage(const std::string& dirPath,
                              const std::string& urlPath);
}

// ── File upload handler ───────────────────────────────────────────────────────
namespace UploadHandler {
    // Parse multipart/form-data from req.body using the boundary from
    // Content-Type. Write each part to loc.uploadStore/filename.
    // Returns 201 with Location header on success; 400 on parse error;
    // 500 on filesystem write failure.
    Response handle(const Request&        req,
                    const LocationConfig& loc);
}

// ── File delete handler ───────────────────────────────────────────────────────
namespace DeleteHandler {
    // access(fsPath, F_OK) → unlink → 204
    // Not found → 404; permission denied → 403
    Response handle(const std::string& fsPath);
}

// ── Multipart parser ──────────────────────────────────────────────────────────
namespace Multipart {
    struct Part {
        std::map<std::string, std::string> headers; // e.g. Content-Disposition
        std::string                        body;
        std::string                        filename; // extracted from Content-Disposition
    };

    // Split a multipart body using the boundary string.
    // Returns one Part per section; returns empty vector on parse failure.
    std::vector<Part> parse(const std::string& body,
                            const std::string& boundary);

    // Extract the boundary string from a Content-Type header value.
    // e.g. "multipart/form-data; boundary=----WebKit" → "----WebKit"
    std::string extractBoundary(const std::string& contentType);
}
