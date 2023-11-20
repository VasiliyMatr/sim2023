# Describe simulator mudule build

add_sim_module(riscv)

target_link_libraries(riscv PUBLIC sim::simulator)

target_sources(riscv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv.cpp)

