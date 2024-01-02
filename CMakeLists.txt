cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/external/grassland/external/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(GameX)

set(CMAKE_CXX_STANDARD 17)

find_package(absl CONFIG REQUIRED)

add_subdirectory(external/grassland)

set(GAMEX_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

add_subdirectory(src/GameX)
add_subdirectory(src/GameBall)

add_subdirectory(test)
