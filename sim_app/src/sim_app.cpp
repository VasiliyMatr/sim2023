#include <iomanip>
#include <vector>
#include <string>
#include <iostream>

#include <sim/common.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>
#include <sim/elf.hpp>

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

    auto simulator = sim::Simulator();
    auto &phys_memory = simulator.getPhysMemory();


    const std::string elf_filename = "/mnt/d/progi/fukn_sim/sim2023/build/sim_app/8queens";

    auto entry_point = elf::load(elf_filename, phys_memory);

    auto status = simulator.simulate(entry_point);

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
