cmake_minimum_required(VERSION 4.0)
project(ChessBot)

set(CMAKE_CXX_STANDARD 23)
set(PROJECT_NAME ChessBot)
set(RAYLIB_VERSION 5.5)

set(EMBEDDED_RESOURCES_HEADER "${CMAKE_CURRENT_BINARY_DIR}/embedded_resources.h"
        src/ChessGame.cpp
        src/ChessGame.h
        src/SoundManager.cpp
        src/SoundManager.h
        src/Bot.cpp)
set(RESOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/move-check.wav"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/move-self.wav"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/illegal.wav"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/capture.mp3"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/promote.mp3"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/game-end.mp3"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/Chess_Pieces_Sprite.png"
)

set(EMBEDDED_RESOURCES "#pragma once\n\n#include <cstddef>\n\n")
foreach (RESOURCE_FILE IN LISTS RESOURCE_FILES)
    get_filename_component(RESOURCE_NAME "${RESOURCE_FILE}" NAME)
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" RESOURCE_SYMBOL "${RESOURCE_NAME}")
    file(READ "${RESOURCE_FILE}" RESOURCE_HEX HEX)
    string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1," RESOURCE_BYTES "${RESOURCE_HEX}")
    string(APPEND EMBEDDED_RESOURCES
            "inline constexpr unsigned char ${RESOURCE_SYMBOL}[] = {${RESOURCE_BYTES}};\n"
            "inline constexpr std::size_t ${RESOURCE_SYMBOL}_size = sizeof(${RESOURCE_SYMBOL});\n\n")
endforeach ()
file(GENERATE OUTPUT "${EMBEDDED_RESOURCES_HEADER}" CONTENT "${EMBEDDED_RESOURCES}")

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

find_package(Catch2 QUIET)
if (NOT Catch2_FOUND)
    include(CTest)
    include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.1
    )

    FetchContent_MakeAvailable(Catch2)
endif ()

add_executable(ChessBotTests
        tests/BoardTests.cpp
        src/Board.cpp
        src/BoardSquare.cpp
        src/Piece.cpp
        src/MoveValidator.cpp
        src/ChessGame.cpp
        src/MoveHistory.cpp
        src/GameState.cpp
        tests/ChessGameTests.cpp
        src/Bot.cpp
)

target_link_libraries(ChessBotTests
        PRIVATE
        raylib
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(ChessBotTests)

add_executable(${PROJECT_NAME} src/main.cpp
        src/Piece.h
        src/BoardSquare.cpp
        src/BoardSquare.h
        src/Board.cpp
        src/Board.h
        src/Piece.cpp
        src/GameState.cpp
        src/GameState.h
        src/MoveHistory.cpp
        src/MoveHistory.h
        src/MoveValidator.cpp
        src/MoveValidator.h
        "${EMBEDDED_RESOURCES_HEADER}"
        src/Bot.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
