cmake_minimum_required(VERSION 3.20)
project(etcd_and_leader_election LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH /usr/local)

find_package(cpprestsdk REQUIRED)
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread) # opsiyonel

include_directories(
    /usr/local/include
    ${Protobuf_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)
link_directories(/usr/local/lib)

add_executable(server main.cpp)
target_link_libraries(server PRIVATE etcd-cpp-api Boost::boost ${Boost_LIBRARIES} cpprestsdk::cpprest)