add_executable(rand_test rand_test.cc)
add_executable(flare_picker_test flare_picker_test.cc)
add_executable(mono_flat_energy_test mono_flat_energy_test.cc)

include_directories(
    ${Geant4_INCLUDE_DIR}
    "${PROJECT_SOURCE_DIR}/include"
)

target_sources(rand_test
    PRIVATE
    rand_test.cc)
target_link_libraries(rand_test ${Geant4_LIBRARIES})

target_sources(flare_picker_test
    PRIVATE flare_picker_test.cc)
target_link_libraries(flare_picker_test ${Geant4_LIBRARIES})

target_sources(mono_flat_energy_test PRIVATE mono_flat_energy_test.cc)
target_link_libraries(mono_flat_energy_test ${Geant4_LIBRARIES})

add_test(rand_test rand_test)
add_test(flare_picker_test flare_picker_test)
