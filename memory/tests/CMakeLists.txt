
if (NOT GTest_FOUND)
    return()
endif()

add_executable(test_memory)

target_link_libraries(test_memory
PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    sim::common
    sim::memory
)

target_sources(test_memory PRIVATE src/main.cpp src/test_phys_memory.cpp)
