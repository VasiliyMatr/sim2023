# Describe hart module build

add_sim_header_module(hart)

target_link_libraries(hart
INTERFACE
    sim::common
    sim::csr
    sim::gpr
    sim::memory
)
