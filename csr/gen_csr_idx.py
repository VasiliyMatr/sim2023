import yaml
import sys
import os

def gen_file_open() -> str:
    return """
        #ifndef INCL_IDX_GEN_HPP
        #define INCL_IDX_GEN_HPP

        #include <cstdint>

        namespace sim::csr {

        enum class CSRIdx : uint16_t {
    """

def gen_enum_value(csr_name: str, csr_idx: int) -> str:
    CSR_NUMBER = 4096

    assert(csr_idx >= 0)
    assert(csr_idx < CSR_NUMBER)

    return "%s = 0x%02x,\n" % (csr_name.upper(), csr_idx)

def gen_file_close() -> str:
    return """
        };

        } // namespace sim::csr

        #endif // INCL_IDX_GEN_HPP
    """

def main():
    IN_NAME = sys.argv[1]
    OUT_NAME = sys.argv[2]

    with open(IN_NAME, 'r') as in_file:
        csrs = yaml.safe_load(in_file)

    out_str = gen_file_open()

    for csr_name, csr_descr in csrs.items():
        csr_idx = csr_descr["idx"]

        out_str += gen_enum_value(csr_name, csr_idx)

    out_str += gen_file_close()

    with open(OUT_NAME, 'w') as out_file:
        out_file.write(out_str)

    os.system("clang-format -i %s" % OUT_NAME)

if __name__ == "__main__":
    main()
