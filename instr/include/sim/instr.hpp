#ifndef INCL_SIM_INSTR_HPP
#define INCL_SIM_INSTR_HPP

#include <cstdint>

#include <sim/common.hpp>

// enum class InstrId
#include <sim/instr/instr_id.gen.hpp>

namespace sim {
namespace instr {

class Instr final {
    InstrId m_id = InstrId::SIM_STATUS_INSTR;
    uint8_t m_rd = 0;
    uint8_t m_rs1 = 0;
    uint8_t m_rs2 = 0;
    uint32_t m_imm = to_underlying(SimStatus::SIM__NOT_IMPLEMENTED_INSTR);
    uint8_t m_rm = 0;

    // Status is stored in imm for status instructions
    static_assert(sizeof(SimStatus) <= sizeof(m_imm));

  public:
    NODISCARD auto id() const noexcept { return m_id; }
    NODISCARD auto rd() const noexcept { return m_rd; }
    NODISCARD auto rs1() const noexcept { return m_rs1; }
    NODISCARD auto rs2() const noexcept { return m_rs2; }
    NODISCARD auto imm() const noexcept { return m_imm; }
    NODISCARD auto rm() const noexcept { return m_rm; }

    Instr() = default;
    Instr(const Instr &) = default;
    Instr &operator=(const Instr &) = default;

    // Decode given InstrCode and construct Instr object
    // Body is generated with YAML description
    explicit Instr(InstrCode instr_code);

    NODISCARD static Instr statusInstr(SimStatus status) noexcept {
        Instr out{};
        out.m_imm = to_underlying(status);
        return out;
    }

    NODISCARD SimStatus status() const noexcept {
        SIM_ASSERT(m_id == InstrId::SIM_STATUS_INSTR);
        return static_cast<SimStatus>(m_imm);
    }
};

} // namespace instr
} // namespace sim

#endif // INCL_SIM_INSTR_HPP
