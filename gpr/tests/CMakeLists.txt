
if (NOT GTest_FOUND)
    return()
endif()

add_executable(test_gpr)

target_link_libraries(test_gpr
PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    sim::common
    sim::gpr
)

target_sources(test_gpr PRIVATE src/main.cpp src/test_gpr.cpp)
