# Describe csr module build

add_sim_header_module(csr)

target_link_libraries(csr
INTERFACE
    sim::common
)
