cmake_minimum_required(VERSION 3.13)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

project(loca_lidar)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(eCAL REQUIRED)
find_package(Protobuf REQUIRED)

set(source_files
  main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/../loca_lidar)

# Collect all C source files from loca_lidar
file(GLOB LOC_LIDAR_SOURCES
    "${CMAKE_SOURCE_DIR}/../../loca_lidar/*.c"
)

add_library(lib_c STATIC ${LOC_LIDAR_SOURCES})

set(protobuf_files

    ${CMAKE_CURRENT_SOURCE_DIR}/../proto/lidar_data.proto

)

add_executable(${PROJECT_NAME} ${source_files})

PROTOBUF_TARGET_CPP(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../proto/ ${protobuf_files})

target_link_libraries(${PROJECT_NAME}
  eCAL::core
  protobuf::libprotobuf
  lib_c
)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)