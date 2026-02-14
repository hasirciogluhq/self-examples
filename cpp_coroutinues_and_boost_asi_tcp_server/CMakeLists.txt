cmake_minimum_required(VERSION 3.20)
project(asio_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)

add_executable(server main.cpp)
target_link_libraries(server Boost::boost)


add_executable(client main_client.cpp)
target_link_libraries(client Boost::boost)