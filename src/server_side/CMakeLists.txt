cmake_minimum_required(VERSION 3.10)
project(configuration_manager_service)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(configuration_manager_service server.cpp)
target_link_libraries(configuration_manager_service PRIVATE sdbus-c++ nlohmann_json::nlohmann_json)