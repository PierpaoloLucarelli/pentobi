#include "crash_handler.h"
#include "logger.h"

#include <csignal>
#include <execinfo.h>
#include <unistd.h>

static void crash_handler(int sig) {
    void* trace[32];
    int size = backtrace(trace, 32);

    Logger::instance().error("Crash signal: ", sig);
    backtrace_symbols_fd(trace, size, STDERR_FILENO);

    _exit(128 + sig);
}

void install_crash_handlers() {
    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGFPE,  crash_handler);
    signal(SIGILL,  crash_handler);
}

