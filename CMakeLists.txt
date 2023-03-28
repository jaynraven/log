cmake_minimum_required(VERSION 3.15)

project(log)

add_definitions("-DUNICODE" "-D_UNICODE")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/code/src/test.cpp)