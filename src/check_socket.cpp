
#include <csignal>
#include "check_socket.hpp"

Check_socket::Check_socket(int socket) : m_socket_fd(socket) {
    if (m_socket_fd < 0) {
        throw std::system_error(Errors::Error_code::invalid_socket);
    }
}

//TODO: need process error
STATUS Check_socket::get_status() const {
    ssize_t read_bytes = -1;
    char buf[M_SIZE_BUFFER] = {0};

    read_bytes = read(m_socket_fd, buf, M_SIZE_BUFFER);
    if (read_bytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            ::fmt::print("not data {}: {}\n", errno, strerror(errno));
            return STATUS::OPEN;
        }
        ::fmt::print("err {}: {}\n", errno, strerror(errno));
        return STATUS::ERROR;
    }
    if (read_bytes == 0) {
        ::fmt::print("close {}: {}\n", errno, strerror(errno));
        return STATUS::CLOSE;
    }
    return ERROR;
}
