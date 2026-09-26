#pragma once
#include <string>
#include "IPoll.hpp"

struct ServerConfig;

// ── One listening TCP socket (one per host:port in config) ───────────────────
class Listener : public IPollHandler {
public:
    Listener(const std::string& host, int port, const ServerConfig* cfg);
    ~Listener();

    // Return the listening fd (already bound, non-blocking, listening).
    int fd() const;

    const ServerConfig* config() const;

    // IPollHandler – only onReadable is meaningful for a listener
    void onReadable(int fd); // accept() new connection, set O_NONBLOCK, add to poll
    void onWritable(int fd); // no-op
    void onError(int fd);    // log and stop accepting

private:
    int                 fd_;
    const ServerConfig* config_;

    // socket() → setsockopt(SO_REUSEADDR) → fcntl(O_NONBLOCK) → bind() → listen()
    void setup(const std::string& host, int port);
};
