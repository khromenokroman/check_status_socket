#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fmt/format.h>
#include <exception>
#include <netinet/tcp.h>
#include <sstream>
#include <thread>

#include "check_socket.hpp"

static constexpr char const *IP_ADDRESS = "127.0.0.1";
static constexpr int PORT = 1993;

struct Keepalive {
    int keepalive = false;
    /*  время после которого начинаются проверки активности соединения, если нет активности. */
    int tcp_keepidle{};
    /* интервал между попытками отправки keepalive проб. */
    int tcp_keepintvl{};
    /* количество неудачных попыток keepalive, после которых считается, что связь потеряна. */
    int tcp_keepcnt{};

    bool operator==(Keepalive const &other) const {
        return other.keepalive == this->keepalive &&
               other.tcp_keepcnt == this->tcp_keepcnt &&
               other.tcp_keepintvl == this->tcp_keepintvl &&
               other.tcp_keepidle == this->tcp_keepidle;
    }

    bool operator!=(Keepalive const &other) const {
        return other.keepalive != this->keepalive ||
               other.tcp_keepcnt != this->tcp_keepcnt ||
               other.tcp_keepintvl != this->tcp_keepintvl ||
               other.tcp_keepidle != this->tcp_keepidle;
    }

    std::string dump() const {
        std::ostringstream oss;
        oss << "{ ";
        oss << "keepalive: " << keepalive << ", ";
        oss << "tcp_keepidle: " << tcp_keepidle << ", ";
        oss << "tcp_keepintvl: " << tcp_keepintvl << ", ";
        oss << "tcp_keepcnt: " << tcp_keepcnt;
        oss << " }";
        return oss.str();
    }
};

static void get_options_socket(int socket_descriptor, Keepalive &keepalive_settings) {
    int ret_val = 0;
    socklen_t ret_val_len = sizeof(ret_val);


    if (getsockopt(socket_descriptor, SOL_SOCKET, SO_KEEPALIVE, &ret_val, &ret_val_len) == -1) {
        throw std::runtime_error(::fmt::format("{}", strerror(errno)));
    }
    keepalive_settings.keepalive = ret_val;
    if (getsockopt(socket_descriptor, IPPROTO_TCP, TCP_KEEPIDLE, &ret_val, &ret_val_len) == -1) {
        throw std::runtime_error(::fmt::format("{}", strerror(errno)));
    }
    keepalive_settings.tcp_keepidle = ret_val;
    if (getsockopt(socket_descriptor, IPPROTO_TCP, TCP_KEEPINTVL, &ret_val, &ret_val_len) == -1) {
        throw std::runtime_error(::fmt::format("{}", strerror(errno)));
    }
    keepalive_settings.tcp_keepintvl = ret_val;
    if (getsockopt(socket_descriptor, IPPROTO_TCP, TCP_KEEPCNT, &ret_val, &ret_val_len) == -1) {
        throw std::runtime_error(::fmt::format("{}", strerror(errno)));
    }
    keepalive_settings.tcp_keepcnt = ret_val;
}

static void set_options_socket(int socket_descriptor, Keepalive const &keepalive_settings) {
    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_KEEPALIVE, &keepalive_settings.keepalive,
                   sizeof(keepalive_settings.keepalive)) != 0) {
        throw std::runtime_error(fmt::format("Can not set settings SO_KEEPALIVE: {}", strerror(errno)));
    }
    if (setsockopt(socket_descriptor, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_settings.tcp_keepidle,
                   sizeof(keepalive_settings.tcp_keepidle)) != 0) {
        throw std::runtime_error(fmt::format("Can not set settings TCP_KEEPIDLE: {}", strerror(errno)));
    }
    if (setsockopt(socket_descriptor, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_settings.tcp_keepintvl,
                   sizeof(keepalive_settings.tcp_keepintvl)) != 0) {
        throw std::runtime_error(fmt::format("Can not set settings TCP_KEEPINTVL: {}", strerror(errno)));
    }
    if (setsockopt(socket_descriptor, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_settings.tcp_keepcnt,
                   sizeof(keepalive_settings.tcp_keepcnt)) != 0) {
        throw std::runtime_error(fmt::format("Can not set settings TCP_KEEPCNT: {}", strerror(errno)));
    }
}

int main() {
    try {
        int sock_fd = -1;
        char buf[4096] = {0};
        ssize_t read_bytes = 0;

        sock_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
        if (sock_fd == -1) {
            throw std::runtime_error(::fmt::format("socket {}", strerror(errno)));
        }
        ::fmt::print("sock_fd is {}\n", sock_fd);
        struct sockaddr_in server{};
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IP_ADDRESS, &(server.sin_addr)) <= 0) {
            throw std::runtime_error(::fmt::format("inet_pton {}", strerror(errno)));
        }
        if (connect(sock_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
            if (errno != EINPROGRESS) {
                throw std::runtime_error(::fmt::format("connect {}", strerror(errno)));
            }
        }
        Keepalive get_sock_settings, need_sock_settings{.keepalive=1, .tcp_keepidle=1, .tcp_keepintvl=1, .tcp_keepcnt=1};
//        set_options_socket(sock_fd, need_sock_settings);

        Check_socket ck_socket(sock_fd);

        while (true) {
            auto res = ck_socket.get_status();
            if(res == STATUS::CLOSE){
                ::fmt::print("Close!\n");
            }
            if (res == STATUS::OPEN){
                continue;
            }
            if (res == STATUS::ERROR){
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    } catch (std::exception const &ex) {
        ::fmt::print("exception: {}\n", ex.what());
        return -1;
    }
}