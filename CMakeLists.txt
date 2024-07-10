add_library(network STATIC src/farfler/network/types.cpp
                           src/farfler/network/pubsub.cpp
                           src/farfler/network/pingpong.cpp
                           src/farfler/network/network.cpp)
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
