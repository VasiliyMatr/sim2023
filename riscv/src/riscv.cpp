#include "sim/memory.hpp"
#include "sim/hart.hpp"
#include "sim/simulator.hpp"
#include "sim/instr.hpp"

constexpr sim::PhysAddr DATA_SEGMENT_BASE_ADDR = 0x5000000000;
constexpr size_t SIZE_16MB = size_t{1} << 24;
constexpr size_t SIZE_2GB = size_t{1} << 31;
namespace sim {

int Main(int argc, char *argv[]) {

    // parse arguments
    sim::memory::PhysMemory memory{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};
    sim::hart::Hart hart{memory};
    sim::Simulator sim{hart, memory};

    // sim.GetProg();

    sim.simulate(DATA_SEGMENT_BASE_ADDR);

    return 0;
}

} // namespace sim

int main(int argc, char *argv[]) {
    return sim::Main(argc, argv);
}