#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
    const char* path = "/tmp/hello.sock";

    // Remove old socket file if it exists
    unlink(path);

    // 1. Create socket (file descriptor)
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 2. Bind socket to a filesystem path
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    // 3. Listen for connections
    listen(server_fd, 1);

    std::cout << "Listening on " << path << std::endl;

    // 4. Accept one client
    int client_fd = accept(server_fd, nullptr, nullptr);

    // 5. Send message
    const char* msg = "Return this one word at a time\n";
    
    size_t total = 0;
    size_t len = std::strlen(msg);

    while (total < len) {
        ssize_t n = write(client_fd, msg + total, len - total);
        if (n <= 0) {
            perror("write");
            break;
        }
        total += n;
    }

    // 6. Cleanup
    close(client_fd);
    close(server_fd);
    unlink(path);

    return 0;
}
