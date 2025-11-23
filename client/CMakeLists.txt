add_executable(client WIN32 main.cpp)

if(WIN32)
    target_link_libraries(client PRIVATE user32 gdi32)
endif()