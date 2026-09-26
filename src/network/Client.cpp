#include "Client.hpp"
#include "PollLoop.hpp"
#include "Handlers.hpp"
#include "Response.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <ctime>

extern PollLoop* g_loop;

static const size_t READ_BUF = 65536; // 64 KB stack buffer per recv

Client::Client(int fd, const ServerConfig* cfg)
    : fd(fd), writeOff(0), config(cfg), lastActivity(std::time(NULL))
{}

void Client::onReadable(int fd) {
    // 1. char buf[READ_BUF]; n = recv(fd, buf, READ_BUF, 0)
    //    if n == -1: onError(fd); return   (no errno inspection)
    //    if n ==  0: onError(fd); return   (peer closed)
    // 2. lastActivity = time(NULL)
    // 3. parser.feed(buf, n)
    // 4. if parser.error():
    //       build Response(parser.request().statusCode)
    //       enqueueResponse(resp.serialize(), *g_loop)
    //       return
    // 5. if parser.done():
    //       Response resp = handle(parser.request(), *config, *g_loop, this)
    //       if resp.status != 0:  // 0 means CGI took over asynchronously
    //           enqueueResponse(resp.serialize(), *g_loop)
    //       parser.reset()
    (void)fd;
}

void Client::onWritable(int fd) {
    // 1. ssize_t n = send(fd, writeBuf.data() + writeOff,
    //                     writeBuf.size() - writeOff, 0)
    //    if n == -1: onError(fd); return
    // 2. writeOff += n
    // 3. if writeOff >= writeBuf.size():
    //       clear writeBuf; writeOff = 0
    //       g_loop->modify(fd, POLLIN)  // stop watching for POLLOUT
    //       close connection (HTTP/1.0 close policy): onError(fd)
    (void)fd;
}

void Client::onError(int fd) {
    // 1. close(fd)
    // 2. g_loop->remove(fd)  — PollLoop will delete this Client after removal
    (void)fd;
}

void Client::enqueueResponse(const std::string& data, IPoll& loop) {
    // 1. writeBuf = data; writeOff = 0
    // 2. loop.modify(this->fd, POLLIN | POLLOUT)
    (void)data; (void)loop;
}

bool Client::isTimedOut(int timeoutSecs) const {
    // return (std::time(NULL) - lastActivity) > timeoutSecs
    (void)timeoutSecs;
    return false;
}
