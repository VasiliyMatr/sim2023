# Describe memory module build

add_sim_module(memory)

target_link_libraries(memory
PUBLIC
    sim::common
    sim::csr
)

target_sources(memory PRIVATE src/memory.cpp)

add_subdirectory(tests)
