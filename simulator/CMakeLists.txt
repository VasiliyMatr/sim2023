# Describe simulator mudule build

add_sim_module(simulator)

set(INCLUDE_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/include/sim/simulator)
file(MAKE_DIRECTORY ${INCLUDE_GEN_DIR})

# Generate dispatch.gen.hpp
add_custom_command(
    OUTPUT ${INCLUDE_GEN_DIR}/dispatch.gen.hpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_dispatch.py ${RISCV_YAML} dispatch.gen.hpp
    WORKING_DIRECTORY ${INCLUDE_GEN_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_dispatch.py ${RISCV_YAML}
)

add_custom_target(gen_dispatch DEPENDS ${INCLUDE_GEN_DIR}/dispatch.gen.hpp)
add_dependencies(simulator gen_dispatch)

target_include_directories(simulator PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/include)

target_link_libraries(simulator
PUBLIC
    sim::bb
    sim::common
    sim::hart
    sim::instr
    sim::cache
)

target_sources(simulator PRIVATE src/simulator.cpp)

add_subdirectory(tests)
