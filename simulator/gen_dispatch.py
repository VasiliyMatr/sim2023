import yaml
import sys
import os

IMPLEMENTED_INSTRS = [
    "ADDI", "SLTI", "SLTIU", "ANDI", "ORI", "XORI",
    "ADDIW", "SLLI", "SRLI", "SRAI", "SLLIW", "SRLIW",
    "SRAIW", "LUI", "AUIPC", "SLL", "SRL", "SRA", "ADDW",
    "SUBW", "SLLW", "SRLW", "SRAW", "LD", "LW", "LWU",
    "LH", "LHU", "LB", "LBU", "SD", "SW", "SH", "SB",
    "JAL", "JALR", "BEQ", "BNE", "BLT", "BLTU", "BGE",
    "BGEU", "ECALL"
]

def gen_file_open() -> str :
    return """
        #ifndef SIMULATOR_DISPATCH_GEN_HPP
        #define SIMULATOR_DISPATCH_GEN_HPP

        namespace sim {

    """

def gen_sim_method_name(mnemonic: str) -> str :
    return "simInstr<instr::InstrId::%s>" % mnemonic

def gen_dispatch(instrs: dict) -> str :
    out = "inline Simulator::SimInstrPtr Simulator::dispatch(instr::InstrId id) noexcept {"

    out += "static constexpr SimInstrPtr DISPATCH_TABLE[] = {\n"

    out += "%s" % gen_sim_method_name("SIM_STATUS_INSTR")

    for instr in instrs :
        mnemonic = instr["mnemonic"].upper().replace('.', '_')
        if mnemonic in IMPLEMENTED_INSTRS :
            out += ", %s" % gen_sim_method_name(mnemonic)
        else :
            out += ", nullptr"

    out += """
            };

            return DISPATCH_TABLE[to_underlying(id)];
        }
    """

    return out

def gen_file_close() -> str :
    return """
        } // namespace sim

        #endif // SIMULATOR_DISPATCH_GEN_HPP

    """

def main() :
    IN_NAME = sys.argv[1]
    OUT_NAME = sys.argv[2]

    with open(IN_NAME, 'r') as file :
        instrs = yaml.safe_load(file)["instructions"]

    out = open(OUT_NAME, 'w')

    out.write(gen_file_open())

    out.write(gen_dispatch(instrs))

    out.write(gen_file_close())

    out.close()
    os.system("clang-format -i %s" % OUT_NAME)

if __name__ == "__main__" :
    main()
