#include <fstream>
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

#ifdef SIM_LOG_ENABLE
    std::ofstream log{"log.txt"};
    auto *log_ptr = &log;
#else
    std::ofstream *log_ptr = nullptr;
#endif

    auto simulator = sim::Simulator(log_ptr);
    auto &pm = simulator.getPhysMemory();

    elf::ElfLoader loader{pm};

    auto [stack_map_status, start_sp] = loader.mapStack();
    SIM_ASSERT(stack_map_status == SimStatus::OK);
    simulator.getHart().gprFile().write(gpr::GPR_IDX::SP, start_sp);

    auto [load_elf_status, start_pc] = loader.loadElf(argv[1]);
    SIM_ASSERT(load_elf_status == SimStatus::OK);

    csr::SATP64 satp64{};
    satp64.setMODE(csr::SATP64::MODEValue::SV39);
    simulator.getHart().csrFile().set(satp64);

    auto status = simulator.simulate(start_pc);

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
