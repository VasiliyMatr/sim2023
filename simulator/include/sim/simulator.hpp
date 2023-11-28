#ifndef INCL_SIM_SIMULATOR_HPP
#define INCL_SIM_SIMULATOR_HPP

#include <type_traits>

#include <sim/bb.hpp>
#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>

namespace sim {

class Simulator final {
    using MemAccessType = memory::MMU64::AccessType;

    memory::PhysMemory m_phys_memory{};

    hart::Hart m_hart{m_phys_memory};

    size_t m_icount = 0;

    // Translate VA -> PA in current privilege level
    template <MemAccessType access_type> auto translateVa(VirtAddr va) {
        return m_hart.mmu64().translate(PrivLevel::USER, access_type, va);
    }

    // Memory load result
    template <class Int> struct LoadResult final {
        SimStatus status = SimStatus::PHYS_MEM__ACCESS_FAULT;
        Int value;
    };

    // Load integer value from memory
    template <class Int, MemAccessType access_type>
    LoadResult<Int> loadInt(VirtAddr va) noexcept {
        static_assert(std::is_integral_v<Int>);

        static_assert(access_type == MemAccessType::FETCH ||
                      access_type == MemAccessType::READ);

        if (va & memory::addrAlignMask<Int>()) {
            return {SimStatus::SIM__UNALIGNED_LOAD, 0};
        }

        // Translate VA -> PA
        auto [mmu_status, pa] = translateVa<access_type>(va);
        if (mmu_status != SimStatus::OK) {
            return {mmu_status, 0};
        }

        // Read physical memory
        Int value = 0;
        auto [read_status, to_cache] = m_hart.physMemory().read(pa, value);
        if (read_status != SimStatus::OK) {
            return {read_status, 0};
        }

        return {SimStatus::OK, value};
    }

    // Simulate load instruction for given type
    template <class Int> SimStatus simLoadInstr(const instr::Instr &instr) {
        static_assert(std::is_integral_v<Int>);

        auto &gpr = m_hart.gprFile();
        auto va = gpr.read<VirtAddr>(instr.rs1());

        auto [status, res] = loadInt<Int, MemAccessType::READ>(va);
        if (status != SimStatus::OK) {
            return status;
        }

        gpr.write(instr.rd(), res);

        ++m_icount;
        m_hart.pc() += INSTR_CODE_SIZE;
        return SimStatus::OK;
    }

    template <class Int> SimStatus storeInt(VirtAddr va, Int value) {
        static_assert(std::is_integral_v<Int>);

        if (va & memory::addrAlignMask<Int>()) {
            return SimStatus::SIM__UNALIGNED_STORE;
        }

        // Translate VA -> PA
        auto [mmu_status, pa] = translateVa<MemAccessType::WRITE>(va);
        if (mmu_status != SimStatus::OK) {
            return mmu_status;
        }

        // Write physical memory
        auto [read_status, to_cache] = m_hart.physMemory().write(pa, value);
        if (read_status != SimStatus::OK) {
            return read_status;
        }

        return SimStatus::OK;
    }

    // Simulate store instruction for given type
    template <class UInt> SimStatus simStoreInstr(const instr::Instr &instr) {
        static_assert(std::is_unsigned_v<UInt>);

        auto &gpr = m_hart.gprFile();
        auto va = gpr.read<VirtAddr>(instr.rs1());

        auto value = gpr.read<UInt>(instr.rs2());

        auto status = storeInt(va, value);

        if (status == SimStatus::OK) {
            ++m_icount;
            m_hart.pc() += INSTR_CODE_SIZE;
        }

        return status;
    }

    template <instr::InstrId>
    SimStatus simInstr(const instr::Instr &instr) noexcept;

    SimStatus simBb(const bb::Bb &bb) noexcept;

    class Fetch final {
        using FetchResult = bb::Bb::FetchResult;

        VirtAddr m_curr_fetch_addr = 0;
        Simulator &m_sim;

      public:
        Fetch(VirtAddr bb_virt_addr, Simulator &sim)
            : m_curr_fetch_addr(bb_virt_addr), m_sim(sim) {}

        FetchResult operator()() noexcept {
            auto res = m_sim.loadInt<InstrCode, MemAccessType::FETCH>(
                m_curr_fetch_addr);

            m_curr_fetch_addr += INSTR_CODE_SIZE;
            return {res.status, res.value};
        }
    };

  public:
    auto &getHart() noexcept { return m_hart; }
    auto &getPhysMemory() noexcept { return m_phys_memory; }

    auto icount() const noexcept { return m_icount; }

    SimStatus simulate(VirtAddr start_pc);
};

} // namespace sim

#endif // INCL_SIM_SIMULATOR_HPP
