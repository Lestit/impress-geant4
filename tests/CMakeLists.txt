add_executable(rand_test rand_test.cc)
add_executable(energy_picker_test energy_picker.cc)

include_directories(
    ${Geant4_INCLUDE_DIR}
    "${PROJECT_SOURCE_DIR}/include"
)

target_sources(rand_test
    PRIVATE
    rand_test.cc)
target_link_libraries(rand_test ${Geant4_LIBRARIES})

target_sources(energy_picker_test
    PRIVATE energy_picker.cc)
target_link_libraries(energy_picker_test ${Geant4_LIBRARIES})

add_test(rand_test rand_test)
add_test(energy_picker_test energy_picker_test)
