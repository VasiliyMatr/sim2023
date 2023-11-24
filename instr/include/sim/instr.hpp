#ifndef INCL_INSTR_HPP
#define INCL_INSTR_HPP

#include <cstdint>

#include <sim/common.hpp>

// enum class InstrId
#include <sim/instr/instr_id.gen.hpp>

namespace sim {
namespace instr {

class Instr final {
    InstrId m_id = InstrId::UNDEF;
    uint8_t m_rd = 0;
    uint8_t m_rs1 = 0;
    uint8_t m_rs2 = 0;
    uint32_t m_imm = 0;
    uint8_t m_rm = 0;

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
};

} // namespace instr
} // namespace sim

#endif // INCL_INSTR_HPP
