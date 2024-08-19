#pragma once

#include <exception>

namespace Errors {
    enum class Error_code {
        invalid_socket
    };

    struct Library : std::error_category {

        [[nodiscard]] char const *name() const noexcept override {
            return "Library check socket error";
        };

        [[nodiscard]] std::string message(int err) const override {
            switch (err) {
                case 0: {
                    return "Socket is not correct status";
                };
                default:
                    throw std::runtime_error("Unknown error!");
            }

        };

        static Library &instance() {
            static Library instance;
            return instance;
        }
    };

    inline std::error_code make_error_code(Error_code err) noexcept {
        return std::error_code{static_cast<int>(err), Library::instance()};
    }
}


namespace std {
    template<>
    struct is_error_code_enum<Errors::Error_code> : true_type {
    };

}

namespace exception {

    struct Library : std::exception {
        explicit Library(std::string message_) : message{std::move(message_)} {};

        [[nodiscard]] const char *what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message{};
    };

    struct Exception : Library {
        using Library::Library;
    };
}