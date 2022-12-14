# just a guess
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(impress)

set(EXC_OUT impress)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
add_executable(${EXC_OUT} impress.cc)

include_directories(
#    ${Geant4_INCLUDE_DIR}
    "${PROJECT_SOURCE_DIR}/include"
)

add_subdirectory(src)
add_subdirectory(include)

target_sources(${EXC_OUT}
    PRIVATE
        impress.cc
        impress.hh
)

target_link_libraries(${EXC_OUT} ${Geant4_LIBRARIES})

# configure data in/out
# file(COPY "flare-data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data-out")

# change copies to symlinks
# flare CDF tables
file(CREATE_LINK "${CMAKE_SOURCE_DIR}/data-tables/flare-cdfs" "${CMAKE_CURRENT_BINARY_DIR}/flare-cdfs" SYMBOLIC)
# config file
file(CREATE_LINK "${CMAKE_SOURCE_DIR}/impress.conf" "${CMAKE_CURRENT_BINARY_DIR}/impress.conf" SYMBOLIC)
# macros
file(CREATE_LINK "${CMAKE_SOURCE_DIR}/macros" "${CMAKE_CURRENT_BINARY_DIR}/macros" SYMBOLIC)

add_compile_options(-Wall -Wextra -Wpedantic -pedantic -Werror)
add_compile_definitions(CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
