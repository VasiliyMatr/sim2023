#ifndef INCL_SIM_BB_HPP
#define INCL_SIM_BB_HPP

#include <array>

#include <sim/instr.hpp>

namespace sim::bb {

struct Bb final {
    static constexpr size_t MAX_SIZE = 16;
    static constexpr VirtAddr INVALID_VA = VirtAddr{1} << 56;

  private:
    VirtAddr m_virt_addr = INVALID_VA;

    size_t m_size = 0;
    std::array<instr::Instr, MAX_SIZE> m_instrs{};

    NODISCARD static constexpr bool isBranch(instr::InstrId id) noexcept {
        switch (id) {
        case instr::InstrId::JAL:
        case instr::InstrId::JALR:
        case instr::InstrId::BEQ:
        case instr::InstrId::BNE:
        case instr::InstrId::BLT:
        case instr::InstrId::BLTU:
        case instr::InstrId::BGE:
        case instr::InstrId::BGEU:
            return true;
        default:
            return false;
        }

        SIM_ASSERT(0);
    }

  public:
    // Basic blocks are considered equal for equal start addrs
    NODISCARD constexpr bool operator==(const Bb &second) const noexcept {
        return second.m_virt_addr == m_virt_addr;
    }

    NODISCARD auto begin() const noexcept {
        SIM_ASSERT(m_virt_addr != INVALID_VA);

        return m_instrs.cbegin();
    }

    NODISCARD auto end() const noexcept {
        SIM_ASSERT(m_virt_addr != INVALID_VA);

        auto it = m_instrs.cbegin();
        std::advance(it, m_size);
        return it;
    }

    struct FetchResult final {
        SimStatus status = SimStatus::PHYS_MEM__ACCESS_FAULT;
        InstrCode instr_code = 0;
    };

    template <class Fetch>
    SimStatus update(VirtAddr bb_virt_addr, Fetch &fetch) noexcept {
        // Decode instrs starting from bb_virt_addr
        m_virt_addr = bb_virt_addr;
        m_size = 0;

        for (; m_size <= Bb::MAX_SIZE;) {
            // Fetch next instr
            FetchResult fetch_res = fetch();

            // Fetch failure ends bb
            if (fetch_res.status != SimStatus::OK) {
                return fetch_res.status;
            }

            // Decode next instr
            auto &instr = m_instrs[m_size] = instr::Instr(fetch_res.instr_code);
            ++m_size;

            // Undefined instr ends bb
            if (instr.id() == instr::InstrId::UNDEF) {
                return SimStatus::SIM__NOT_IMPLEMENTED_INSTR;
            }

            // Branch instr ends bb
            if (isBranch(instr.id())) {
                return SimStatus::OK;
            }
        }

        // Reached max size
        return SimStatus::OK;
    }

    void constexpr invalidate() noexcept {
        m_size = 0;
        m_virt_addr = INVALID_VA;
    }
};

} // namespace sim::bb

#endif // INCL_SIM_BB_HPP
