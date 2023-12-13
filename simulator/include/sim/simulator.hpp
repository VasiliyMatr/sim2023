#ifndef INCL_SIM_SIMULATOR_HPP
#define INCL_SIM_SIMULATOR_HPP

#include <type_traits>

#include <sim/bb.hpp>
#include <sim/bb_cache.hpp>
#include <sim/common.hpp>
#include <sim/hart.hpp>
#include <sim/instr.hpp>
#include <sim/memory.hpp>
#include <sim/tlb.hpp>

namespace sim {

class Simulator final {
    using MemAccessType = memory::MMU64::AccessType;

    static constexpr size_t TLB_SIZE_LOG_2 = 7;
    static constexpr size_t BB_CACHE_SIZE_LOG_2 = 7;

    using ReadTLB = cache::TLB<memory::ConstHostPtr, TLB_SIZE_LOG_2>;
    using WriteTLB = cache::TLB<memory::HostPtr, TLB_SIZE_LOG_2>;

    memory::PhysMemory m_phys_memory{};

    hart::Hart m_hart{m_phys_memory};

    ReadTLB m_read_tlb {};
    WriteTLB m_write_tlb {};
    ReadTLB m_fetch_tlb {};

    cache::BbCache<BB_CACHE_SIZE_LOG_2> m_bb_cache;

    size_t m_icount = 0;

    // Translate VA -> PA in current privilege level
    template <MemAccessType access_type> auto translateVa(VirtAddr va) {
        return m_hart.mmu64().translate(PrivLevel::USER, access_type, va);
    }

    // Memory load result
    template <class Int> struct LoadResult final {
        SimStatus status = SimStatus::PHYS_MEM__ACCESS_FAULT;
        Int value = 0;
    };

    template<MemAccessType access_type>
    constexpr auto &getReadTLB() noexcept {
        if (access_type == MemAccessType::FETCH) {
            return m_fetch_tlb;
        }
        return m_read_tlb;
    }

    // Load integer value from memory
    template <class Int, MemAccessType access_type>
    LoadResult<Int> loadInt(VirtAddr va) noexcept {
        static_assert(std::is_integral_v<Int>);

        static_assert(access_type == MemAccessType::FETCH ||
                      access_type == MemAccessType::READ);

        // Check alignment
        if (va & memory::addrAlignMask<Int>()) {
            return {SimStatus::SIM__UNALIGNED_LOAD, 0};
        }

        // Try to hit tlb
        memory::ConstHostPtr host_addr = nullptr;
        if (getReadTLB<access_type>().find(va, host_addr)) {
            Int value = *reinterpret_cast<const Int *>(host_addr);
            return {SimStatus::OK, value};
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

        // Cache tranlation
        getReadTLB<access_type>().update(va, to_cache);

        return {SimStatus::OK, value};
    }

    // Simulate load instruction for given type
    template <class Int> SimStatus simLoadInstr(const instr::Instr *instr) {
        static_assert(std::is_integral_v<Int>);

        auto &gpr = m_hart.gprFile();
        auto va_base = gpr.read<VirtAddr>(instr->rs1());
        auto va = va_base + static_cast<int32_t>(instr->imm());

        auto [status, res] = loadInt<Int, MemAccessType::READ>(va);
        if (status != SimStatus::OK) {
            return status;
        }

        gpr.write(instr->rd(), res);

        ++m_icount;
        m_hart.pc() += INSTR_CODE_SIZE;
        return SimStatus::OK;
    }

    template <class Int> SimStatus storeInt(VirtAddr va, Int value) {
        static_assert(std::is_integral_v<Int>);

        // Check alignment
        if (va & memory::addrAlignMask<Int>()) {
            return SimStatus::SIM__UNALIGNED_STORE;
        }

        // Try to hit tlb
        memory::HostPtr host_addr = nullptr;
        if (m_write_tlb.find(va, host_addr)) {
            *reinterpret_cast<Int *>(host_addr) = value;
            return SimStatus::OK;
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

        // Cache translation
        m_write_tlb.update(va, to_cache);

        return SimStatus::OK;
    }

    // Simulate store instruction for given type
    template <class UInt> SimStatus simStoreInstr(const instr::Instr *instr) {
        static_assert(std::is_unsigned_v<UInt>);

        auto &gpr = m_hart.gprFile();
        auto va_base = gpr.read<VirtAddr>(instr->rs1());
        auto va = va_base + static_cast<int32_t>(instr->imm());

        auto value = gpr.read<UInt>(instr->rs2());

        auto status = storeInt(va, value);
        if (status != SimStatus::OK) {
            return status;
        }

        ++m_icount;
        m_hart.pc() += INSTR_CODE_SIZE;
        return SimStatus::OK;
    }

    template <class Int, template <typename> typename Cmp>
    SimStatus simCondBranch(const instr::Instr *instr) {
        auto &gpr = m_hart.gprFile();

        auto rs1 = gpr.read<Int>(instr->rs1());
        auto rs2 = gpr.read<Int>(instr->rs2());

        if (Cmp<Int>()(rs1, rs2)) {
            auto offset = static_cast<int64_t>(instr->imm());
            auto new_pc = m_hart.pc() + offset;

            if (new_pc & 0x3) {
                return SimStatus::SIM__PC_ALIGN_ERROR;
            }

            ++m_icount;
            m_hart.pc() = new_pc;
            return SimStatus::OK;
        }

        ++m_icount;
        m_hart.pc() += INSTR_CODE_SIZE;
        return SimStatus::OK;
    }

    template <instr::InstrId>
    static SimStatus simInstr(Simulator &sim,
                              const instr::Instr *instr) noexcept;

    using SimInstrPtr = SimStatus (*)(Simulator &, const instr::Instr *);

    inline static SimInstrPtr dispatch(instr::InstrId) noexcept;

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
