#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>
#include <sim/simulator.hpp>
namespace sim {

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
    if (status == sim::Simulator::SimStatus::OK) {
        return 0;
    }
    if (status == sim::Simulator::SimStatus::NOT_IMPLEMENTED_INSTR) {
        std::cout << "NOT_IMPLEMENTED_INSTR";
    }
    std::cerr << "Runtime Error." << std::endl;
    return 1;
}

} // namespace sim

int main() { return sim::Main(); }
