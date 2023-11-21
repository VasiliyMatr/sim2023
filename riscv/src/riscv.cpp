#include "sim/memory.hpp"
#include "sim/hart.hpp"
#include "sim/simulator.hpp"
#include "sim/instr.hpp"

constexpr sim::PhysAddr DATA_SEGMENT_BASE_ADDR = 0x5000000000;
constexpr size_t SIZE_16MB = size_t{1} << 24;
constexpr size_t SIZE_2GB = size_t{1} << 31;
namespace sim {

enum RiscV {
    OK,
    PARSE_ERROR,
    RUNTIME_ERROR,
};

sim::RiscV Main(int argc, char *argv[]) {

    // parse arguments
    if (argc != 2) {
        std::cerr << "There are not enough arguments. Enter size of array.";
        return RiscV::PARSE_ERROR;
    }

    int mode = std::atoi(argv[1]); // mode to switch to elf mode in future
    if (mode != 1) {
        std::cerr << "ELF is not supported yet.";
        return RiscV::PARSE_ERROR;
    }

    std::vector<sim::InstrCode> binaryInstructions = {
        0x00000293, // lw t0, num1
        0x00100293, // lw t1, num2
        0x00228213, // add t2, t0, t1
        0x00220293, // sw t2, result
        0x0000006f  // return
    };

    sim::memory::PhysMemory memory{DATA_SEGMENT_BASE_ADDR, SIZE_16MB};
    sim::hart::Hart hart{memory};
    sim::Simulator sim{hart, memory};
    sim.addInstructionsToMemory(binaryInstructions, memory, binaryInstructions.size());
    sim::Simulator::SimStatus status = sim.simulate(DATA_SEGMENT_BASE_ADDR);
    if (status == sim::Simulator::SimStatus::OK) {
        return RiscV::OK;
    }
    return RiscV::RUNTIME_ERROR;
}

} // namespace sim

int main(int argc, char *argv[]) {
    return sim::Main(argc, argv);
}