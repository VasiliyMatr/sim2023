#include <sim/memory.hpp>
#include <sim/hart.hpp>
#include <sim/simulator.hpp>
#include <sim/instr.hpp>
namespace sim {

int Main() {

    std::vector<sim::InstrCode> binaryInstructions = {
        0x00000293, // lw t0, num1
        0x00100293, // lw t1, num2
        0x00228213, // add t2, t0, t1
        0x00220293, // sw t2, result
        0x0000006f  // return
    };

    auto simulator = sim::Simulator();
    simulator.addInstructionsToMemory(binaryInstructions, binaryInstructions.size());
    auto status = simulator.simulate(DATA_SEGMENT_BASE_ADDR);
    if (status == sim::Simulator::SimStatus::OK) {
        return 0;
    }
    std::cerr << "Runtime Error." << std::endl;
    return 1;
}

} // namespace sim

int main() {
    return sim::Main();
}