cmake_minimum_required(VERSION 3.10)
project(ChessGame)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(a.out main.cpp game.cpp piece.cpp chess.cpp)

target_link_libraries(a.out sfml-graphics sfml-window sfml-system)