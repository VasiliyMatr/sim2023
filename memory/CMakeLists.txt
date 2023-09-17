# Describe memory module build

add_sim_header_module(memory)

target_link_libraries(memory
INTERFACE
    sim::common
)
