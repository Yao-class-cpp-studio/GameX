set(LIB_NAME GameXUtils)

find_package(assimp CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES *.cpp *.h)

add_library(${LIB_NAME} ${SOURCES}
        common.cpp
        common.h)

target_include_directories(${LIB_NAME} PUBLIC ${COMMON_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} PUBLIC grassland assimp::assimp)

list(APPEND GAMEX_LIBS ${LIB_NAME})
set(GAMEX_LIBS ${GAMEX_LIBS} PARENT_SCOPE)

target_compile_definitions(${LIB_NAME} PUBLIC GAMEX_ASSETS_DIR="${GAMEX_ASSETS_DIR}/")
