import yaml
import sys


def main():
    if len(sys.argv) < 2:
        print("No risc-v.yaml file provided")
        return
    
    with open(sys.argv[1]) as f:
        yaml_dump = dict(yaml.safe_load(f))

    write_buffer = "#ifndef INCL_TYPE_GEN_HPP" "\n" +\
                   "#define INCL_TYPE_GEN_HPP" "\n" +\
                   "#include <cstdint>" "\n" +\
                   "namespace sim {" "\n" +\
                   "namespace instr {" "\n" +\
                   "enum class InstrId : uint8_t {" "\n"

    write_buffer += "UNDEF,\n"
    for inst in yaml_dump.get("instructions"):
        inst_name = inst.get("mnemonic").upper()
        if "." in inst_name:
            inst_name = inst_name.replace(".", "_", 2)

        write_buffer += f"{inst_name},\n"

    write_buffer += "};\n\n"
    write_buffer += "} // namespace instr" "\n"
    write_buffer += "} // namespace sim" "\n"

    write_buffer += "#endif // INCL_TYPE_GEN_HPP" "\n"

    gen = open("instr_id.gen.hpp", "w+")

    gen.write(write_buffer)


if __name__ == "__main__":
    main()
