# Describe hart module build

add_sim_module(hart src/hart.cpp)

target_link_libraries(hart
PRIVATE
    sim::csr
    sim::gpr
    sim::memory
)
