#pragma once

// ── Event handler interface ───────────────────────────────────────────────────
// Any object registered with IPoll must implement this.
class IPollHandler {
public:
    virtual ~IPollHandler() {}

    // Called when poll() reports the fd readable (POLLIN).
    virtual void onReadable(int fd) = 0;

    // Called when poll() reports the fd writable (POLLOUT).
    virtual void onWritable(int fd) = 0;

    // Called on POLLHUP / POLLERR, or when the loop decides to close the fd.
    virtual void onError(int fd) = 0;
};

// ── Poll-loop registration interface ────────────────────────────────────────
// abood uses this to register CGI pipe fds without touching aqahwaji's internals.
class IPoll {
public:
    virtual ~IPoll() {}

    // Register fd with the given poll events (POLLIN | POLLOUT etc.) and handler.
    virtual void add(int fd, short events, IPollHandler* handler) = 0;

    // Change the events monitored for an already-registered fd.
    virtual void modify(int fd, short events) = 0;

    // Unregister fd and stop monitoring it.
    virtual void remove(int fd) = 0;
};
