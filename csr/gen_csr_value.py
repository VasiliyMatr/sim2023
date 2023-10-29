import yaml
import sys
import os

def gen_file_open() -> str:
    return """
        #ifndef INCL_VALUE_GEN_HPP
        #define INCL_VALUE_GEN_HPP

        #include <sim/csr/idx.gen.hpp>
        #include <sim/csr/raw_value.hpp>

        namespace sim::csr {

        template <XLen xlen, CSRIdx idx> class CSRValue;

    """

def gen_field_enum(enum_name: str, enum_descr: dict[int, str]) -> str:
    out = "enum class %s {\n" % enum_name

    for value, value_name in enum_descr.items():
        out += "%s = %d,\n" % (value_name.upper(), value)

    out += "};\n\n"
    return out

def gen_field_code(field_name: str, field_descr: dict | list[int]) -> str:
    out = ""

    upper_field_name = field_name.upper()

    complex_descr = type(field_descr) == dict

    enum_name = "%sValue" % upper_field_name

    enum_descr: dict | None = field_descr.get("enum", None) if complex_descr else None
    (hi, lo) = field_descr.get("bits") if complex_descr else field_descr
    field_len = hi - lo

    field_value = "RawValue" if enum_descr == None else enum_name

    if enum_descr != None:
        out += gen_field_enum(enum_name, enum_descr)

    # Write getter
    out += "auto get%s() const noexcept {\n" % upper_field_name
    out += "raw_value = bit::getBitField<RawValue, %d, %d>(getValue());" % (hi, lo)
    out += "return " + ("raw_value;\n" if enum_descr == None else "%s{};\n" % enum_name)
    out += "}\n"

    # Write setter
    out += "void set%s(%s value) noexcept {\n" % (upper_field_name, field_value)
    out += "auto mask = (RawValue{1} << %d) - (RawValue{1} << %d);\n" % (hi, lo)
    out += "value = (value << %d) & mask;\n" % lo
    out += "setValue((getValue() & ~mask) | value);\n"
    out += "}\n\n"

    return out

def gen_csr_spec(csr_name: str, xlen: int, spec_descr: dict) -> str:
    assert(xlen == 32 or xlen == 64)

    xlen_str = "XLen::XLEN_%d" % xlen
    base_str = "BaseCSRValue%d" % xlen

    out = "template<> struct CSRValue<CSRIdx::%s, %s> : public %s {\n" % (csr_name.upper(), xlen_str, base_str)
    out += "using %s::RawValue;\n\n" % base_str

    for field_name, field_descr in spec_descr.items():
        out += gen_field_code(field_name, field_descr)

    out += "};\n\n"

    return out

def gen_csr_code(csr_name: str, csr_descr: dict) -> str:
    xlen32_descr: dict = csr_descr["xlen32"]
    xlen64_descr: dict = csr_descr["xlen64"]

    return gen_csr_spec(csr_name, 32, xlen32_descr) + gen_csr_spec(csr_name, 64, xlen64_descr)

def gen_file_close() -> str:
    return """
        } // namespace sim::csr

        #endif // INCL_VALUE_GEN_HPP
    """

def main():
    IN_NAME = sys.argv[1]
    OUT_NAME = sys.argv[2]

    with open(IN_NAME, 'r') as in_file:
        csrs = yaml.safe_load(in_file)

    out_str = gen_file_open()

    for csr_name, csr_descr in csrs.items():
        out_str += gen_csr_code(csr_name, csr_descr)

    out_str += gen_file_close()

    with open(OUT_NAME, 'w') as out_file:
        out_file.write(out_str)

    os.system("clang-format -i %s" % OUT_NAME)

if __name__ == "__main__":
    main()
