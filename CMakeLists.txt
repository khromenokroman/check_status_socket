cmake_minimum_required(VERSION 3.28)

find_package(fmt)
project(check_status_socket)

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        ${PROJECT_SOURCE_DIR}/src/check_socket.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
