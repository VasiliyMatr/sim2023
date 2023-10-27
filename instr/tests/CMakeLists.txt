
if (NOT GTest_FOUND)
    return()
endif()

add_executable(test_instr)

target_link_libraries(test_instr
PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    sim::common
    sim::instr
)

target_sources(test_instr PRIVATE src/main.cpp src/test_instr_decoding.cpp ../generated/src/decode.gen.cpp)
