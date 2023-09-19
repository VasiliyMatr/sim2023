
if (NOT GTest_FOUND)
    return()
endif()

add_executable(test_common)

target_link_libraries(test_common
PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    sim::common
)

target_sources(test_common PRIVATE src/main.cpp src/test_bit.cpp)
