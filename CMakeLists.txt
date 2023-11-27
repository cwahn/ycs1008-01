cmake_minimum_required(VERSION 3.0.0)
project(ycs-1008-01-ws-server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

include_directories(lib/asio-1.28.0/include)

add_executable(ycs-1008-01-ws-server server.cpp)
target_link_libraries(ycs-1008-01-ws-server PRIVATE
    efp_log 
    Websockets
    )

add_executable(ycs-1008-01-ws-client client.cpp)
target_link_libraries(ycs-1008-01-ws-client PRIVATE
    efp_log 
    Websockets
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(lib)
