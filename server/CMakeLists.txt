add_executable(server main.cpp server.cpp)

if(WIN32)
    target_link_libraries(server PRIVATE user32 gdi32 ws2_32)
endif()