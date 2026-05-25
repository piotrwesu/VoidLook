cmake_minimum_required(VERSION 3.26)

set(PROJECTNAME "VoidLook")

project(${PROJECTNAME} VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL REQUIRED)
find_package(PkgConfig QUIET)

pkg_check_modules(LIBSIXEL REQUIRED IMPORTED_TARGET libsixel)

if(NOT LIBSIXEL_FOUND)
    message(FATAL_ERROR
        "Can't find libsixel, need to install it.")
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.env)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/.env.example" DESTINATION "${CMAKE_BINARY_DIR}/source")
    file(RENAME "${CMAKE_BINARY_DIR}/.env.example" "${CMAKE_BINARY_DIR}/source/.env")
else()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/.env" DESTINATION "${CMAKE_BINARY_DIR}/source")
endif()

add_subdirectory(external)
add_subdirectory(source)
