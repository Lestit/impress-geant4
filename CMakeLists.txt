# just a guess
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(impress)

set(EXC_OUT impress)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
add_executable(${EXC_OUT} impress.cc)

enable_testing()
add_subdirectory(tests)

include_directories(
    ${Geant4_INCLUDE_DIR}
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

# copy flare CDF tables
file(COPY "data-tables/flare-cdfs" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
# copy element spectrum tables
file(COPY "data-tables/element-intens" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

# copy macros
file(COPY macros DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

add_compile_options(-Wall -Wextra -Wpedantic -pedantic -Werror)
