# Describe gpr module build

add_sim_header_module(gpr)

target_link_libraries(gpr
INTERFACE
    sim::common
)

add_subdirectory(tests)
