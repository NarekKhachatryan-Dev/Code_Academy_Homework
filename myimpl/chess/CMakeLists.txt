cmake_minimum_required(VERSION 3.10)
project(ChessboardProject)

set(CMAKE_CXX_STANDARD 17)

add_executable(a.out
    main.cpp
    chess.cpp
    piece.cpp
    game.cpp
)