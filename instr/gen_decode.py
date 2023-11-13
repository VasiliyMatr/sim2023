import yaml
import subprocess
import sys

KNOWN_REGS = ["rd", "rs1", "rs2", "rm"]
KNOWN_IMMS = [ "imm12", "imm20", "jimm20", "storeimm", "bimm", "shamt", "shamtw"]
IMMS_NO_EXTEND = ["shamt", "shamtw"]

def GenerateGetBinValue(bit_section: dict) -> str:
    write_buffer = ""
    write_buffer += "bit::getBitField<InstrCode,"
    write_buffer += f"{bit_section.get('msb')},"
    write_buffer += f"{bit_section.get('lsb')}>(instr_code)"

    shift = bit_section.get('to')
    if shift != 0:
        write_buffer += f" << {shift}"
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

        if field in KNOWN_IMMS and field not in IMMS_NO_EXTEND and extend_from <= 30:
            write_buffer += f"m_imm = bit::signExtend<InstrCode ,{extend_from}>(m_imm);\n"

    return write_buffer


def GenerateHandleNode(node_dict : dict, field_dict: dict) -> str:
    write_buffer = ""

    for key, value in node_dict.items():

        if type(key) is int:
            write_buffer += f"case {key}:\n"
            if value.get("range") is not None:
                write_buffer += GenerateHandleNode(value, field_dict)
            elif value.get("mnemonic") is not None:
                write_buffer += GenerateFieldAssigning(value, field_dict)

            write_buffer += "break;\n"

        elif key == "range":
            write_buffer += "switch ("
            write_buffer += "bit::getBitField<InstrCode,"
            write_buffer += f"{value.get('msb')},"
            write_buffer += f"{value.get('lsb')}>(instr_code)"
            write_buffer += ")\n"

        elif key == "nodes":
            write_buffer += "{\n"
            write_buffer += GenerateHandleNode(value, field_dict)
            write_buffer += "default:\nbreak;"
            write_buffer += "}\n"

        else:
            print("Unknown key:", key)

    return write_buffer


def GenerateDecoderTree(yaml_dump: dict) -> str:
    write_buffer = ""
    write_buffer += GenerateHandleNode(yaml_dump.get("decodertree"), yaml_dump.get("fields"))

    return write_buffer


def main():
    if len(sys.argv) < 2:
        print("No risc-v.yaml file provided")
        return
    with open(sys.argv[1]) as f:
        yaml_dump = dict(yaml.safe_load(f))

    write_buffer = "#include <sim/instr.hpp>" "\n"  +\
        "#include <sim/common.hpp> \n"              +\
        "#include <iostream> \n"              +\
        "\n"                                        +\
        "namespace sim {" "\n"                      +\
        "namespace instr {" "\n"                    +\
        "\n"                                        +\
        "Instr::Instr(InstrCode instr_code) {" "\n"

    write_buffer += GenerateDecoderTree(yaml_dump)

    write_buffer += "}\n"
    write_buffer += "}" "\n"
    write_buffer += "}" "\n"

    with open("decode.gen.cpp", "w+") as f:
        f.write(write_buffer)

    subprocess.run("clang-format -i ./decode.gen.cpp", shell=True)


if __name__ == "__main__":
    main()
