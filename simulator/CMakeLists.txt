# Describe simulator mudule build

add_sim_module(simulator)

target_link_libraries(simulator
PRIVATE:
    sim::common
    sim::hart
    sim::instr
)

target_sources(simulator PRIVATE src/simulator.cpp)
