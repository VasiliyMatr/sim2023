#include "sim/common.hpp"
#include <cstdint>
#include <iomanip>

#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>

namespace sim {

namespace {

void dump_gpr_file(const gpr::GPRFile &gpr_file) {
    std::cout << std::setfill('0');

    for (size_t i = 0; i != gpr::GPR_NUMBER; ++i) {
        std::cout << std::dec << "[" << std::setw(2) << i << "]"
                  << " = ";

        std::cout << "0x" << std::hex << std::setw(sizeof(uint64_t) * 2);

        std::cout << gpr_file.read<uint64_t>(i) << std::endl;
    }

    std::cout << std::setfill(' ') << std::dec;
}

} // namespace

int Main() {
    std::vector<InstrCode> binaryInstructions = {
        0x00a0059b, // addiw a1,x0,10
        0x0140051b, // addiw a0,x0,20
        0x00b5053b, // addw a0,a0,a1
        0x40a5853b, // subw a0,a1,a0
        0x05d0089b, // addiw a7,x0,93
        0x00000073  // ecall
    };

    auto simulator = sim::Simulator();
    simulator.loadToMemory(binaryInstructions, PHYS_MEM_BASE_ADDR);
    auto status = simulator.simulate(PHYS_MEM_BASE_ADDR);

    std::cout << "GPRs:" << std::endl;
    dump_gpr_file(simulator.getHart().gprFile());

    switch (status) {
    case sim::Simulator::SimStatus::OK:
        std::cout << "Success!" << std::endl;
        return 0;
    default:
        std::cout << "Error!" << std::endl;
        return -1;
    }

    SIM_ASSERT(0);
}

} // namespace sim

int main() { return sim::Main(); }
