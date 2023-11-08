# Describe instr module build

add_sim_module(instr)

target_link_libraries(instr
PUBLIC
    sim::common
)

# Generate instr_id.gen.hpp
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/include/sim/instr)
add_custom_command(
    OUTPUT instr_id.gen.hpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_instr_id.py ${CMAKE_CURRENT_SOURCE_DIR}/risc-v.yaml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/include/sim/instr
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_instr_id.py ${CMAKE_CURRENT_SOURCE_DIR}/risc-v.yaml
)

add_custom_target(gen_instr_id DEPENDS instr_id.gen.hpp)
add_dependencies(instr gen_instr_id)

target_include_directories(instr PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/include)

# Generate decode.gen.cpp
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/src)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/src/decode.gen.cpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_decode.py ${CMAKE_CURRENT_SOURCE_DIR}/risc-v.yaml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/src
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_decode.py ${CMAKE_CURRENT_SOURCE_DIR}/risc-v.yaml
)

target_sources(instr PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/src/decode.gen.cpp)

add_subdirectory(tests)
