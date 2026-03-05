cmake_minimum_required(VERSION 3.20)
project(etcd_and_leader_election LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS thread) # opsiyonel

include_directories(
    /usr/local/include
    ./src/**/*.hpp
    ${Boost_INCLUDE_DIRS}
)
link_directories(/usr/local/lib)

# main.cpp + election.cpp
add_executable(app
    src/main.cpp
)

target_link_libraries(app PRIVATE Boost::boost Boost::thread ${Boost_LIBRARIES})