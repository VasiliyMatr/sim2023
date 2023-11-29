# Describe memory module build

add_sim_header_module(cache_tlb)

target_link_libraries(cache_tlb
INTERFACE
    sim::common
    sim::memory
)
