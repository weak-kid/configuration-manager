cmake_minimum_required(VERSION 3.10)
project(conf_manager_application)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(conf_manager_application client.cpp)
target_link_libraries(conf_manager_application PRIVATE sdbus-c++ nlohmann_json::nlohmann_json)