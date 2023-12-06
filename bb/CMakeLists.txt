# Describe bb module build

add_sim_header_module(bb)

target_link_libraries(bb
INTERFACE
    sim::instr
)
