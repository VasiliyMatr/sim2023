#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <sim/common.hpp>
#include <sim/elf_load.hpp>
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

int main(int argc, char **argv) {
    SIM_ASSERT(argc == 2);

    auto simulator = sim::Simulator();
    auto &pm = simulator.getPhysMemory();

    auto sp = elf::map_stack(pm);
    simulator.getHart().gprFile().write(gpr::GPR_IDX::SP, sp);
    auto entry_point = elf::load(argv[1], pm);

    auto status = simulator.simulate(entry_point);

    std::cout << "icount = " << simulator.icount() << std::endl;

    std::cout << "GPRs:" << std::endl;
    dump_gpr_file(simulator.getHart().gprFile());

    switch (status) {
    case SimStatus::OK:
        std::cout << "Success" << std::endl;
        return 0;
    default:
        std::cout << "Error: " << sim::to_underlying(status) << std::endl;
        return -1;
    }

    SIM_UNREACHABLE();
}
