file(GLOB_RECURSE SOURCES *.cpp *.h)

add_executable(GameBall ${SOURCES})
target_link_libraries(GameBall PRIVATE GameX absl::flags absl::flags_parse)
target_link_libraries(GameBall PRIVATE Sound)

# Delete main.cpp from sources
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Create a library for the tests
add_library(GameBallLib ${SOURCES})
target_link_libraries(GameBallLib PRIVATE GameX)
