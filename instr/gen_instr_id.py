
gen = open("instr_id.gen.hpp", "w")

gen.write(
    "#ifndef INCL_TYPE_GEN_HPP" "\n"
    "#define INCL_TYPE_GEN_HPP" "\n"

    "namespace sim {" "\n"
    "namespace instr {" "\n"

    "enum class InstrId : uint16_t {" "\n"
    "    UNDEF," "\n"
    "    NOP," "\n"
    "    ADDI," "\n"
    # ....
    "};" "\n"

    "} // namespace instr" "\n"
    "} // namespace sim" "\n"

    "#endif // INCL_TYPE_GEN_HPP" "\n"
)
