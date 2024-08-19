#include "check_socket.hpp"

Check_socket::Check_socket(int socket) : m_socket(socket) {
    if (m_socket < 0){
        throw std::runtime_error()
    }
}
