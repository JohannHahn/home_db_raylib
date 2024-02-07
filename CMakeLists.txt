cmake_minimum_required(VERSION 3.12)

project(layout)

add_subdirectory(raylib)
add_executable(home_db sqlite3.c main.cpp data_base.cpp)

set_property(TARGET home_db PROPERTY CXX_STANDARD 20)

target_link_libraries(home_db raylib)

