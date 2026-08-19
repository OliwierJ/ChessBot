cmake_minimum_required(VERSION 4.0)
project(ChessBot)

set(CMAKE_CXX_STANDARD 23)
set(PROJECT_NAME ChessBot)
set(RAYLIB_VERSION 5.5)

find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG ${RAYLIB_VERSION}
    )
    FetchContent_MakeAvailable(raylib)
endif ()

add_executable(${PROJECT_NAME} main.cpp
        Piece.h
        BoardSquare.cpp
        BoardSquare.h
        Board.cpp
        Board.h
        Piece.cpp
        GameState.cpp
        GameState.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
