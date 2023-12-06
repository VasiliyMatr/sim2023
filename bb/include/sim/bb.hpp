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

    NODISCARD const auto *instrs() const noexcept { return m_instrs.data(); }

    struct FetchResult final {
        SimStatus status = SimStatus::PHYS_MEM__ACCESS_FAULT;
        InstrCode instr_code = 0;
    };

    template <class Fetch>
    void update(VirtAddr bb_virt_addr, Fetch &fetch) noexcept {
        // Decode instrs starting from bb_virt_addr
        m_virt_addr = bb_virt_addr;

        for (size_t i = 0; i < MAX_SIZE - 1; ++i) {
            // Fetch next instr
            FetchResult fetch_res = fetch();

            // Fetch failure ends bb
            if (fetch_res.status != SimStatus::OK) {
                m_instrs[i] = instr::Instr::statusInstr(fetch_res.status);
                return;
            }

            // Decode next instr
            auto &instr = m_instrs[i] = instr::Instr(fetch_res.instr_code);

            // Status instr indicates illegal instr
            // Illegal instr ends bb
            if (instr.id() == instr::InstrId::SIM_STATUS_INSTR) {
                return;
            }

            // Branch instr ends bb
            if (isBranch(instr.id())) {
                return;
            }
        }

        // Reached max size
        m_instrs[MAX_SIZE - 1] = instr::Instr::statusInstr(SimStatus::OK);
    }

    void invalidate() noexcept {
        m_virt_addr = INVALID_VA;
        m_instrs[0] =
            instr::Instr::statusInstr(SimStatus::SIM__NOT_IMPLEMENTED_INSTR);
    }
};

} // namespace sim::bb

#endif // INCL_SIM_BB_HPP
