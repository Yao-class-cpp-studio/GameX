cmake_minimum_required(VERSION 3.22)

add_library(Sound PlaySound.cpp PlaySound.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(Sound PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/irrKlang.lib)

install(TARGETS Sound
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})