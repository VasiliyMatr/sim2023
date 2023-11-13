# Describe instr module build

add_sim_module(instr)

target_link_libraries(instr
PUBLIC
    sim::common
)

set(RISCV_YAML ${CMAKE_CURRENT_SOURCE_DIR}/risc-v.yaml)
set(INCLUDE_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/include/sim/instr)
set(SRC_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/src)

file(MAKE_DIRECTORY ${INCLUDE_GEN_DIR})
file(MAKE_DIRECTORY ${SRC_GEN_DIR})

# Generate instr_id.gen.hpp
add_custom_command(
    OUTPUT ${INCLUDE_GEN_DIR}/instr_id.gen.hpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_instr_id.py ${RISCV_YAML}
    WORKING_DIRECTORY ${INCLUDE_GEN_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_instr_id.py ${RISCV_YAML}
)

add_custom_target(gen_instr_id DEPENDS ${INCLUDE_GEN_DIR}/instr_id.gen.hpp)
add_dependencies(instr gen_instr_id)

target_include_directories(instr PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/include)

# Generate decode.gen.cpp
add_custom_command(
    OUTPUT ${SRC_GEN_DIR}/decode.gen.cpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_decode.py ${RISCV_YAML}
    WORKING_DIRECTORY ${SRC_GEN_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_decode.py ${RISCV_YAML}
)

target_sources(instr PRIVATE ${SRC_GEN_DIR}/decode.gen.cpp)

add_subdirectory(tests)
