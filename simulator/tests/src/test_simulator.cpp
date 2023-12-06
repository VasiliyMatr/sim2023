#include <vector>

#include <gtest/gtest.h>

#include <sim/simulator.hpp>

namespace sim {

class SimulatorTest : public ::testing::Test {
  protected:
    static constexpr PhysAddr CODE_SEG_BASE = 0x5000000000;

    Simulator sim{};

    SimStatus simulate(const std::vector<InstrCode> &code) {
        auto &phys_memory = sim.getPhysMemory();

        for (PhysAddr page_pa = CODE_SEG_BASE,
                      end = code.size() + CODE_SEG_BASE;
             page_pa < end; page_pa += memory::PAGE_SIZE) {
            SIM_ASSERT(phys_memory.addRAMPage(page_pa));
        }

        for (size_t i = 0, end = code.size(); i != end; ++i) {
            SIM_ASSERT(
                phys_memory.write(CODE_SEG_BASE + i * INSTR_CODE_SIZE, code[i])
                    .status == SimStatus::OK);
        }

        return sim.simulate(CODE_SEG_BASE);
    }
};

TEST_F(SimulatorTest, ecall) {
    const std::vector<InstrCode> CODE = {
        0x05d0089b, // addiw a7,x0,93
        0x00000073  // ecall
    };

    ASSERT_EQ(simulate(CODE), SimStatus::OK);
    ASSERT_EQ(sim.icount(), CODE.size());

    ASSERT_EQ(sim.getHart().gprFile().read<uint64_t>(gpr::GPR_IDX::A7), 93);
}

TEST_F(SimulatorTest, addSub) {
    const std::vector<InstrCode> CODE = {
        0x00a0059b, // addiw a1,x0,10
        0x0140051b, // addiw a0,x0,20
        0x00b5053b, // addw a0,a0,a1
        0x40a5853b, // subw a0,a1,a0

        0x05d0089b, // addiw a7,x0,93
        0x00000073  // ecall
    };

    ASSERT_EQ(simulate(CODE), SimStatus::OK);
    ASSERT_EQ(sim.icount(), CODE.size());

    const auto &gpr = sim.getHart().gprFile();

    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::A0), -20);
    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::A1), 10);
}

TEST_F(SimulatorTest, cycle) {
    const std::vector<InstrCode> CODE = {
        0x0000051b, // addiw a0, zero, 0
        0x0000029b, // addiw t0, zero, 0
        0x0050031b, // addiw t1, zero, 5

        // for:
        0x0062d863, // bge t0, t1, end
        0x0055053b, // addw a0, a0, t0
        0x0012829b, // addiw t0, t0, 1
        0xff5ff06f, // j for

        // end:
        0x05d0089b, // addiw a7, x0, 93
        0x00000073  // ecall
    };

    ASSERT_EQ(simulate(CODE), SimStatus::OK);
    ASSERT_EQ(sim.icount(), 26);

    const auto &gpr = sim.getHart().gprFile();

    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::A0), 10);
    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::T0), 5);
    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::T1), 5);
}

TEST_F(SimulatorTest, loadStore) {
    const PhysAddr DATA_PAGE_PA = 0x6000000000;

    ASSERT_TRUE(sim.getPhysMemory().addRAMPage(DATA_PAGE_PA));

    const std::vector<InstrCode> CODE = {
        0x1bf0051b, // addiw   a0, zero, 0x1BF
        0x0060059b, // addiw   a1, zero, 6
        0x02459593, // slli    a1, a1, 36

        0x00358593, // addi    a1, a1, 3
        0x00a5b2a3, // sd      a0, 5(a1)

        0x00858593, // addi    a1, a1, 8
        0xffd5b603, // ld      a2, -3(a1)

        0x05d0089b, // addiw a7, x0, 93
        0x00000073  // ecall
    };

    ASSERT_EQ(simulate(CODE), SimStatus::OK);
    ASSERT_EQ(sim.icount(), CODE.size());

    const auto &gpr = sim.getHart().gprFile();

    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::A1), DATA_PAGE_PA + 11);
    ASSERT_EQ(gpr.read<uint64_t>(gpr::GPR_IDX::A2), 0x1BF);
}

} // namespace sim
