# Describe instr module build

add_sim_module(instr)

target_link_libraries(instr
PRIVATE
    sim::common
)

# Generate instr_id.gen.hpp
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generation/include/sim/instr)
add_custom_command(
    OUTPUT instr_id.gen.hpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_instr_id.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generation/include/sim/instr
)

add_custom_target(gen_instr_id DEPENDS instr_id.gen.hpp)
add_dependencies(instr gen_instr_id)

target_include_directories(instr PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generation/include)

# Generate decode.gen.cpp
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generation/src)
add_custom_command(
    OUTPUT decode.gen.cpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_decode.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generation/src
)

add_custom_target(gen_decode DEPENDS decode.gen.cpp)
add_dependencies(instr gen_decode)

set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/generation/src/decode.gen.cpp PROPERTY GENERATED 1)

target_sources(instr PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generation/src/decode.gen.cpp)

