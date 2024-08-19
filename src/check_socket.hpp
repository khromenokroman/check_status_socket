#include <fmt/format.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "exceptions.hpp"

enum STATUS {
    OPEN,
    CLOSE,
    ERROR
};

class Check_socket final {
public:
    explicit Check_socket(int socket);

    [[nodiscard]] STATUS get_status() const;

    static constexpr uint16_t M_SIZE_BUFFER = 4096;
private:
    int m_socket_fd = -1;
};
