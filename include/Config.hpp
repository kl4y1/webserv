#pragma once
#include <string>
#include <vector>
#include <map>

// ── Per-location block ──────────────────────────────────────────────────────
struct LocationConfig {
    std::string              prefix;        // e.g. "/uploads"
    std::vector<std::string> methods;       // ["GET","POST"]
    std::string              root;          // filesystem root for this location
    std::string              index;         // default file (e.g. "index.html")
    std::string              uploadStore;   // directory for uploads
    std::string              cgiExtension;  // e.g. ".py"
    std::string              cgiPath;       // interpreter path e.g. "/usr/bin/python3"
    std::string              returnUrl;     // if set, respond with redirect
    int                      returnCode;    // e.g. 301
    bool                     autoindex;     // directory listing on/off
    long                     clientMaxBodySize; // bytes, 0 = unlimited

    LocationConfig();
};

// ── Per-server block ────────────────────────────────────────────────────────
struct ServerConfig {
    std::string                    host;          // e.g. "0.0.0.0"
    int                            port;          // e.g. 8080
    std::vector<std::string>       serverNames;   // virtual-host names
    std::map<int, std::string>     errorPages;    // code → URI
    long                           clientMaxBodySize;
    std::vector<LocationConfig>    locations;

    ServerConfig();
};

// ── Top-level parser ─────────────────────────────────────────────────────────
class ConfigParser {
public:
    // Parse file at path; throws std::runtime_error on any error.
    // Returns one ServerConfig per server{} block found.
    static std::vector<ServerConfig> parse(const std::string& path);

private:
    // Tokenize the raw file text into words, '{', '}', ';'
    static std::vector<std::string> tokenize(const std::string& text);

    // Advance token index and parse one server{} block
    static ServerConfig parseServer(const std::vector<std::string>& tokens,
                                    size_t& i);

    // Advance token index and parse one location{} block
    static LocationConfig parseLocation(const std::vector<std::string>& tokens,
                                        size_t& i);

    // Read a directive value tokens until ';'; advance i past it
    static std::string expectValue(const std::vector<std::string>& tokens,
                                   size_t& i);
};
