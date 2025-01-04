cmake_minimum_required(VERSION 3.10)
project(2048 C)

set(CMAKE_C_STANDARD 23)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

add_executable(2048 src/main.c)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
)