#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

// ── Default constructors ──────────────────────────────────────────────────────
LocationConfig::LocationConfig()
    : returnCode(0), autoindex(false), clientMaxBodySize(1024 * 1024)
{}

ServerConfig::ServerConfig()
    : port(80), clientMaxBodySize(1024 * 1024)
{}

// ── ConfigParser::parse ───────────────────────────────────────────────────────
std::vector<ServerConfig> ConfigParser::parse(const std::string& path) {
    // 1. Open path; throw std::runtime_error if it can't be opened
    // 2. Read entire file into a std::string
    // 3. tokens = tokenize(fileContent)
    // 4. Walk tokens:
    //    if token == "server" → parseServer(tokens, i); push result
    //    else throw "unexpected token outside server block"
    // 5. Return vector of ServerConfigs
    (void)path;
    return std::vector<ServerConfig>();
}

// ── ConfigParser::tokenize ────────────────────────────────────────────────────
std::vector<std::string> ConfigParser::tokenize(const std::string& text) {
    // Walk text character by character:
    //   '#' → skip until newline (comment)
    //   '{' or '}' → push as single-character token
    //   ';' → push as single-character token
    //   whitespace → flush current word token if any
    //   other → accumulate into current word
    // Return all tokens in order
    (void)text;
    return std::vector<std::string>();
}

// ── ConfigParser::parseServer ─────────────────────────────────────────────────
ServerConfig ConfigParser::parseServer(const std::vector<std::string>& tokens,
                                       size_t& i) {
    // Expect '{' at tokens[i]; advance i
    // Loop until '}':
    //   "listen"                 → parse "host:port" or just "port"; set srv.host, srv.port
    //   "host"                   → expectValue → srv.host
    //   "server_name"            → read words until ';'; push into srv.serverNames
    //   "error_page"             → read code + uri; srv.errorPages[code] = uri
    //   "client_max_body_size"   → expectValue → parse number (support K/M suffix)
    //   "location"               → read prefix + '{'; parseLocation(tokens,i); push
    //   '}'                      → break
    //   anything else            → throw "unknown directive: <token>"
    // Return srv
    (void)tokens; (void)i;
    return ServerConfig();
}

// ── ConfigParser::parseLocation ───────────────────────────────────────────────
LocationConfig ConfigParser::parseLocation(const std::vector<std::string>& tokens,
                                            size_t& i) {
    // Expect the prefix string has already been read by parseServer.
    // Expect '{' at tokens[i]; advance i
    // Loop until '}':
    //   "methods"              → read words until ';'; push into loc.methods
    //   "return"               → read code + url; loc.returnCode, loc.returnUrl
    //   "root"                 → expectValue → loc.root
    //   "autoindex"            → expectValue; "on" → true, "off" → false
    //   "index"                → expectValue → loc.index
    //   "upload_store"         → expectValue → loc.uploadStore
    //   "cgi_extension"        → expectValue → loc.cgiExtension
    //   "cgi_path"             → expectValue → loc.cgiPath
    //   "client_max_body_size" → expectValue → parse number
    //   '}'                    → break
    //   anything else          → throw "unknown location directive: <token>"
    // Return loc
    (void)tokens; (void)i;
    return LocationConfig();
}

// ── ConfigParser::expectValue ─────────────────────────────────────────────────
std::string ConfigParser::expectValue(const std::vector<std::string>& tokens,
                                       size_t& i) {
    // Advance i; if tokens[i] == ';' or '}' throw "expected value"
    // Save tokens[i] as value; advance i past the ';'
    // Return value
    (void)tokens; (void)i;
    return "";
}
