#pragma once
#include <string>
#include <ctime>
#include <sys/types.h>
#include "IPoll.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

class Client;

// ── One in-flight CGI execution ───────────────────────────────────────────────
class CgiSession : public IPollHandler {
public:
    static const int TIMEOUT_SECS = 10;

    // Launch the CGI process for the given request and location.
    // Registers stdinFd_ and stdoutFd_ on the poll loop.
    // When complete, delivers the response to the owning Client.
    CgiSession(const Request&        req,
               const LocationConfig& loc,
               const std::string&    scriptPath,
               Client*               client,
               IPoll&                loop);

    ~CgiSession();

    // IPollHandler – called by PollLoop when the pipe fds are ready
    void onReadable(int fd); // read CGI stdout into fromChild_; on EOF parse response
    void onWritable(int fd); // write toChild_ body to CGI stdin; close pipe when done
    void onError(int fd);    // kill child, return 502

    // Check if the child has been running longer than TIMEOUT_SECS.
    // Call kill(pid_, SIGKILL) + waitpid + deliver 504.
    void checkTimeout();

    pid_t pid() const;

private:
    pid_t       pid_;
    int         stdinFd_;      // write end of stdin pipe (parent side)
    int         stdoutFd_;     // read end of stdout pipe (parent side)
    std::string toChild_;      // request body to feed to CGI stdin
    size_t      writeOff_;     // bytes of toChild_ already written
    std::string fromChild_;    // accumulated CGI output
    std::time_t startedAt_;
    Client*     client_;
    IPoll&      loop_;

    // Build the CGI environment variable array from the request + location.
    // Caller must free each string and the array.
    char** buildEnv(const Request& req, const LocationConfig& loc,
                    const std::string& scriptPath) const;

    // Parse raw CGI output in fromChild_: split headers from body,
    // map CGI headers to HTTP headers, build a Response.
    Response parseCgiOutput() const;

    // Deliver response to the client via client_->enqueueResponse()
    void deliver(const Response& resp);
};
