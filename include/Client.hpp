#pragma once
#include <string>
#include <ctime>
#include "Request.hpp"
#include "IPoll.hpp"

struct ServerConfig;

// ── One connected TCP client ──────────────────────────────────────────────────
struct Client : public IPollHandler {
    int               fd;
    std::string       readBuf;      // bytes received but not yet fully parsed
    std::string       writeBuf;     // serialized response waiting to be sent
    size_t            writeOff;     // how many bytes of writeBuf already sent
    RequestParser     parser;       // owns the incremental parse state
    const ServerConfig* config;     // which server block accepted this client
    std::time_t       lastActivity; // updated on every recv; used for timeout

    explicit Client(int fd, const ServerConfig* cfg);

    // IPollHandler – called by PollLoop
    void onReadable(int fd); // recv up to 64 KB, feed to parser, trigger handler
    void onWritable(int fd); // send next chunk of writeBuf, disable POLLOUT when empty
    void onError(int fd);    // close and clean up

    // Enqueue a serialized response; enables POLLOUT on the poll loop
    void enqueueResponse(const std::string& data, IPoll& loop);

    // True if the client has been idle longer than timeoutSecs
    bool isTimedOut(int timeoutSecs) const;
};
