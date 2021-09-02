# just a guess
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(impress)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${Geant4_INCLUDE_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
# includes headers and "impress.hh"
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/*.hh)

add_executable(impress impress.cc ${sources} ${headers})
target_link_libraries(impress ${Geant4_LIBRARIES})

# copy over macros
file(GLOB macros "${PROJECT_SOURCE_DIR}/macros/*.mac")

foreach (macro ${macros})
    get_filename_component(barename ${macro} NAME)
    configure_file(
        ${macro}
        ${CMAKE_CURRENT_BINARY_DIR}/macros/${barename}
        COPYONLY)
endforeach()
