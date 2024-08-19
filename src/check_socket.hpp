#include <fmt/format.h>

#include "exceptions.hpp"

class Check_socket final {
public:
    explicit Check_socket(int socket);

private:
    int m_socket = -1;
};
