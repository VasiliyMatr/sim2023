#include "sim/common.hpp"
#include <iomanip>
#include <vector>

#include <sim/memory.hpp>
#include <sim/simulator.hpp>

using namespace sim;

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

int main() {
    const PhysAddr CODE_SEG_BASE = 0x5000000000;

    const std::vector<InstrCode> CODE = {
        0x00a0059b, // addiw a1,x0,10
        0x0140051b, // addiw a0,x0,20
        0x00b5053b, // addw a0,a0,a1
        0x40a5853b, // subw a0,a1,a0
        0x05d0089b, // addiw a7,x0,93
        0x00000073  // ecall
    };

    auto simulator = sim::Simulator();
    auto &phys_memory = simulator.getPhysMemory();

    for (PhysAddr page_pa = CODE_SEG_BASE, end = CODE.size() + CODE_SEG_BASE;
         page_pa < end; page_pa += memory::PAGE_SIZE) {
        SIM_ASSERT(phys_memory.addRAMPage(page_pa));
    }

    for (size_t i = 0, end = CODE.size(); i != end; ++i) {
        SIM_ASSERT(
            phys_memory.write(CODE_SEG_BASE + i * INSTR_CODE_SIZE, CODE[i])
                .status == SimStatus::OK);
    }

    auto status = simulator.simulate(CODE_SEG_BASE);

    std::cout << "GPRs:" << std::endl;
    dump_gpr_file(simulator.getHart().gprFile());

    switch (status) {
    case SimStatus::OK:
        std::cout << "Success!" << std::endl;
        return 0;
    default:
        std::cout << "Error!" << std::endl;
        return -1;
    }

    SIM_ASSERT(0);
}
