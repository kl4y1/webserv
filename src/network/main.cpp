#include "Config.hpp"
#include "PollLoop.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

volatile sig_atomic_t g_stop = 0;
PollLoop*             g_loop = NULL;

static void sigintHandler(int) {
    g_stop = 1;
}

int main(int argc, char* argv[]) {
    try {
        // 1. Determine config path:
        //    argc >= 2 → argv[1], else "./default.conf"
        std::string configPath = (argc >= 2) ? argv[1] : "./default.conf";

        // 2. Parse config file; throws on any syntax or semantic error
        std::vector<ServerConfig> configs = ConfigParser::parse(configPath);
        if (configs.empty()) {
            std::cerr << "No server blocks found in " << configPath << "\n";
            return 1;
        }

        // 3. Ignore SIGPIPE (broken pipe on send() must not crash the server)
        signal(SIGPIPE, SIG_IGN);

        // 4. Install SIGINT handler that only sets g_stop
        signal(SIGINT, sigintHandler);

        // 5. Build and start the poll loop
        PollLoop loop;
        g_loop = &loop;
        loop.init(configs);
        loop.run();

        // 6. run() returns when g_stop is set; cleanup happens in destructors
        std::cout << "\nShutting down.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Fatal: unknown exception\n";
        return 1;
    }
}
