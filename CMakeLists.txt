add_library(network STATIC src/farfler/network/types.cpp
                           src/farfler/network/pubsub.cpp)
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
