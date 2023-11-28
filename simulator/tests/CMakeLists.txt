if (NOT GTest_FOUND)
    return()
endif()

add_executable(test_simulator)

target_link_libraries(test_simulator
PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    sim::simulator
)

target_sources(test_simulator PRIVATE src/main.cpp src/test_simulator.cpp)
