#include "Cgi.hpp"
#include "Client.hpp"
#include "PollLoop.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

extern PollLoop* g_loop;

CgiSession::CgiSession(const Request&        req,
                       const LocationConfig& loc,
                       const std::string&    scriptPath,
                       Client*               client,
                       IPoll&                loop)
    : pid_(-1), stdinFd_(-1), stdoutFd_(-1), writeOff_(0),
      startedAt_(std::time(NULL)), client_(client), loop_(loop)
{
    // 1. int stdinPipe[2], stdoutPipe[2]; pipe(stdinPipe); pipe(stdoutPipe)
    // 2. pid_ = fork()
    // Child:
    //   a. close read end of stdinPipe; close write end of stdoutPipe
    //   b. dup2(stdinPipe[0], STDIN_FILENO)
    //   c. dup2(stdoutPipe[1], STDOUT_FILENO)
    //   d. close remaining pipe ends
    //   e. chdir to script directory (dirname of scriptPath)
    //   f. build argv: {interpreter, scriptPath, NULL}
    //   g. buildEnv(req, loc, scriptPath)
    //   h. execve(loc.cgiPath.c_str(), argv, envp)
    //   i. _exit(1) if execve fails
    // Parent:
    //   a. close child ends: stdinPipe[0], stdoutPipe[1]
    //   b. stdinFd_  = stdinPipe[1];  set O_NONBLOCK
    //   c. stdoutFd_ = stdoutPipe[0]; set O_NONBLOCK
    //   d. toChild_  = req.body
    //   e. if toChild_ not empty: loop_.add(stdinFd_,  POLLOUT, this)
    //      else: close(stdinFd_); stdinFd_ = -1
    //   f. loop_.add(stdoutFd_, POLLIN, this)
    (void)req; (void)loc; (void)scriptPath;
}

CgiSession::~CgiSession() {
    // Close any still-open pipe fds
    // Do NOT waitpid here — checkTimeout or onError already did it
}

void CgiSession::onReadable(int fd) {
    // fd == stdoutFd_
    // char buf[4096]; ssize_t n = read(fd, buf, sizeof(buf))
    // if n > 0: fromChild_.append(buf, n)
    // if n == 0 (EOF):
    //   loop_.remove(stdoutFd_); close(stdoutFd_); stdoutFd_ = -1
    //   waitpid(pid_, NULL, 0)
    //   deliver(parseCgiOutput())
    //   delete this
    // if n == -1: onError(fd) (pipe error)
    (void)fd;
}

void CgiSession::onWritable(int fd) {
    // fd == stdinFd_
    // ssize_t n = write(fd, toChild_.data() + writeOff_,
    //                   toChild_.size() - writeOff_)
    // if n > 0: writeOff_ += n
    // if writeOff_ >= toChild_.size():
    //   loop_.remove(stdinFd_); close(stdinFd_); stdinFd_ = -1
    //   (done writing body to CGI; stdout pipe stays open for reading)
    // if n == -1: onError(fd)
    (void)fd;
}

void CgiSession::onError(int fd) {
    // Close any open pipe fds, remove from loop
    // kill(pid_, SIGKILL); waitpid(pid_, NULL, 0)
    // deliver(Response(502))
    // delete this
    (void)fd;
}

void CgiSession::checkTimeout() {
    // if (time(NULL) - startedAt_) > TIMEOUT_SECS:
    //   kill(pid_, SIGKILL); waitpid(pid_, NULL, 0)
    //   if stdinFd_  >= 0: loop_.remove(stdinFd_);  close(stdinFd_)
    //   if stdoutFd_ >= 0: loop_.remove(stdoutFd_); close(stdoutFd_)
    //   deliver(Response(504))
    //   delete this
}

pid_t CgiSession::pid() const { return pid_; }

char** CgiSession::buildEnv(const Request&        req,
                             const LocationConfig& loc,
                             const std::string&    scriptPath) const {
    // Build a std::vector<std::string> of "KEY=VALUE" strings:
    //   GATEWAY_INTERFACE=CGI/1.1
    //   SERVER_PROTOCOL=HTTP/1.1
    //   SERVER_NAME=<from request Host header>
    //   SERVER_PORT=<loc port if accessible, else empty>
    //   REQUEST_METHOD=<req.method>
    //   SCRIPT_FILENAME=<scriptPath>
    //   PATH_INFO=<req.path>
    //   QUERY_STRING=<req.query>
    //   CONTENT_LENGTH=<req.body.size() or "" if GET>
    //   CONTENT_TYPE=<req.headers["Content-Type"] or "">
    //   For each req.header: HTTP_<UPPERCASED_NAME>=<value>
    //     (replace '-' with '_' in header name)
    // Allocate char** of size(vec)+1; last entry = NULL
    // Caller must free each element and the array
    (void)req; (void)loc; (void)scriptPath;
    return NULL;
}

Response CgiSession::parseCgiOutput() const {
    // Split fromChild_ at the first blank line "\r\n\r\n" or "\n\n"
    // Parse CGI headers (before blank line):
    //   "Status: NNN ..." → use NNN as response status; default 200 if absent
    //   "Content-Type: ..." → copy to response headers
    //   other CGI headers → copy to response headers
    // Body = everything after the blank line
    // Return assembled Response
    return Response(200);
}

void CgiSession::deliver(const Response& resp) {
    // Serialize resp and call client_->enqueueResponse(data, loop_)
    (void)resp;
}
