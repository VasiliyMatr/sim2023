# Describe cache module build

add_sim_header_module(cache)

target_link_libraries(cache
INTERFACE
    sim::common
    sim::memory
)
