cmake_minimum_required(VERSION 3.1.0)

project(thefrisbee VERSION 1.0.0 LANGUAGES CXX)

find_package(QT REQUIRED NAMES Qt6)

add_subdirectory(libthefrisbee)
add_subdirectory(application)
