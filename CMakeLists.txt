cmake_minimum_required(VERSION 3.13)
project(farfler_network)

option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)

add_library(network STATIC src/farfler/network/types.cpp
                           src/farfler/network/pubsub.cpp
                           src/farfler/network/pingpong.cpp
                           src/farfler/network/network.cpp)
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(network PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(BUILD_PYTHON_BINDINGS)
    find_package(pybind11 REQUIRED)
    find_package(Boost REQUIRED COMPONENTS system)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${PYBIND11_INCLUDE_DIRS})
    include_directories(${Python3_INCLUDE_DIRS})

    pybind11_add_module(farfler_network src/python/bindings.cpp)
    target_link_libraries(farfler_network PRIVATE network ${Boost_LIBRARIES} pybind11::module)
    target_include_directories(farfler_network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    set_target_properties(farfler_network PROPERTIES OUTPUT_NAME "farfler_network")
endif()
