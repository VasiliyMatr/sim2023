import yaml
import subprocess

KNOWN_REGS = ["rd", "rs1", "rs2", "rm"]
KNOWN_IMMS = [ "imm12", "imm20", "jimm20", "storeimm", "bimm", "shamt", "shamtw"]

def GenerateGetBinValue(bit_section: dict) -> str:
    write_buffer = ""
    write_buffer += "getBitField<InstrCode,"
    write_buffer += f"{bit_section.get('msb')},"
    write_buffer += f"{bit_section.get('lsb')}>(instr_code)"

    shift = bit_section.get('to')
    if shift != 0:
        write_buffer += f" << Word({shift})"
    return write_buffer


def GenerateFieldAssigning(inst: dict, field_dict: dict) -> str:
    write_buffer = ""
    write_buffer += f"m_id = InstrId::{inst.get('mnemonic').upper().replace('.', '_', 3)};\n"
    extend_from = 0 

    for field in inst.get("fields"):
        if field in KNOWN_REGS:
            write_buffer += f"m_{field} = static_cast<uint8_t>("
            write_buffer += GenerateGetBinValue(field_dict.get(field).get('location').get('bits')[0])
            write_buffer += ");\n"
        elif field in KNOWN_IMMS:
            for bit_section in field_dict.get(field).get('location').get('bits'):
                write_buffer += f"m_imm |= static_cast<uint32_t>("
                extend_from = max(extend_from, bit_section.get('from'))
                write_buffer += GenerateGetBinValue(bit_section)
                write_buffer += ");\n"
        
        if field in KNOWN_IMMS:
            write_buffer += f"m_imm = signExtend<{extend_from + 1}>(m_imm);\n"
    
    return write_buffer


def GenerateSwitchCaseMasks(yaml_dump: dict) -> str:
    write_buffer = ""
    masks = dict(dict())
    for inst in yaml_dump.get("instructions"):
        if masks.get(inst.get("debug_hex_fixedmask")) == None:
            masks[inst.get("debug_hex_fixedmask")] = {inst.get("mnemonic"):inst}
        else:
            masks[inst.get("debug_hex_fixedmask")][inst.get("mnemonic")] = inst

            
    for mask, inst_dict in masks.items():
        write_buffer += f"swicth (instr_code & 0x{mask}) {{\n"
        for inst_name, inst in inst_dict.items():
            match = inst.get("debug_hex_fixedvalue")

            write_buffer += f"case 0x{match}:\n"
            write_buffer += GenerateFieldAssigning(inst, yaml_dump.get("fields"))
            write_buffer += "return decoded_instr;\n"

        write_buffer += "default:\nbreak;\n}\n"


    return write_buffer

def main():

    with open("risc-v.yaml") as f:
        yaml_dump = dict(yaml.safe_load(f))

    write_buffer = "#include <sim/instr.hpp>" "\n"  +\
        "\n"                                        +\
        "namespace sim {" "\n"                      +\
        "namespace instr {" "\n"                    +\
        "\n"                                        +\
        "Instr::Instr(InstrCode instr_code) {" "\n"

    write_buffer += GenerateSwitchCaseMasks(yaml_dump)    

    write_buffer += "}\n"
    write_buffer += "}" "\n"
    write_buffer += "}" "\n"

    with open("decode.gen.cpp", "w+") as f:
        f.write(write_buffer)
    
    subprocess.run("clang-format -i ./decode.gen.cpp", shell=True)


if __name__ == "__main__":
    main()
