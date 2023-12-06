import yaml
import sys
import os

def gen_raw_field_type(field_len: int) -> str:
    assert(field_len > 0)
    assert(field_len <= 64)

    if field_len <= 8:
        return "uint8_t"
    if field_len <= 16:
        return "uint16_t"
    if field_len <= 32:
        return "uint32_t"

    return "uint64_t"

class CSRField:
    # TODO: support def __init__(self, name: str, field_descr: dict | list) 
    def __init__(self, name: str, field_descr):
        self.name = name.upper()
        self.m_name = "m_%s" % name.lower()

        complex_descr = type(field_descr) == dict
        (self.hi, self.lo) = field_descr["bits"] if complex_descr else field_descr

        self.raw_type = gen_raw_field_type(1 + self.hi - self.lo)

        self.enum_values = field_descr.get("enum", None) if complex_descr else None
        self.is_enum = self.enum_values != None

        self.type = "%sValue" % self.name if self.is_enum else self.raw_type

        self.default = "%s::%s" % (self.type, self.enum_values[0]) if self.is_enum else "0"

    def gen_enum(self) -> str:
        if not self.is_enum:
            return ""

        out = "enum class %s : %s {\n" % (self.type, self.raw_type)

        for value, value_name in self.enum_values.items():
            out += "%s = %d,\n" % (value_name.upper(), value)

        out += "};\n\n"

        return out

    def gen_is_legal(self) -> str:
        if not self.is_enum:
            return ""

        out = "NODISCARD static bool is%sLegal(%s raw_value) noexcept {\n" %\
            (self.name, self.raw_type)

        out += "switch(%s(raw_value)) {" % self.type

        for value_name in self.enum_values.values():
            out += "\n"
            out += "case %s::%s:" % (self.type, value_name.upper())

        out += """
                    return true;
                }
                return false;
            };

        """

        return out

class CSRValue:
    def __init__(self, name: str, idx_name: str, xlen: int, csr_descr: dict):
        assert(xlen == 32 or xlen == 64)

        self.name = name.upper()
        self.idx_name = idx_name
        self.xlen = xlen

        self.fields = [CSRField(field_name, field_descr)\
            for field_name, field_descr in csr_descr.items()]

    def gen_enums_code(self):
        out = ""

        for field in self.fields:
            out += field.gen_enum()
            out += field.gen_is_legal()

        return out

    def gen_m_fields(self):
        out = ""

        for field in self.fields:
            out += "%s %s = %s;\n" % (field.type, field.m_name, field.default)

        return out

    def gen_set_value(self):
        out = "void setValue(RawValue raw_value) {\n"

        for field in self.fields:
            get_bit_field = "bit::getBitField(%d, %d, raw_value)" %\
                (field.hi, field.lo)

            if field.is_enum:
                raw_field_value = "raw_%s_value" % (field.name.lower())

                out += "auto %s = %s;\n" % (raw_field_value, get_bit_field)
                out += "if (!is%sLegal(%s)) {\n" % (field.name, raw_field_value)
                out += "%s = 0;\n" % raw_field_value
                out += "}\n"
                out += "%s = %s(%s);\n" % (field.m_name, field.type, raw_field_value)

            else:
                out += "%s = %s;\n" % (field.m_name, get_bit_field)

        out += "}\n"

        return out

    def gen_get_value(self) -> str:
        out = """
            NODISCARD auto getValue() const noexcept {
            RawValue value = 0;
        """

        for field in self.fields:
            field_value = "static_cast<%s>(%s)" % (field.raw_type, field.m_name) if field.is_enum\
                else field.m_name

            out += "value = bit::setBitField<RawValue>(%d, %d, value, %s);\n" %\
                (field.hi, field.lo, field_value)

        out += """
            return value;
            }
        """

        return out

    def gen_fields_set_get(self) -> str:
        out = ""

        for field in self.fields:
            out += "NODISCARD auto get%s() const noexcept {\n" % field.name
            out += "return %s;\n" % field.m_name
            out += "}\n"

            out += "void set%s(%s value) noexcept {\n" % (field.name, field.type)
            out += "%s = value;\n" % field.m_name
            out += "}\n\n"

        return out

    def gen_code(self) -> str:
        xlen_str = "XLen::XLEN_%d" % self.xlen

        out = "template<> struct CSRValue<%s, CSRIdx::%s> {\n" % (xlen_str, self.idx_name)
        out += "using RawValue = RawCSRValue<%s>;\n\n" % xlen_str

        out += self.gen_enums_code()

        out += "private:\n"
        out += self.gen_m_fields()
        out += "\n"
        out += "public:\n"

        out += """
            CSRValue(RawValue raw_value = 0) {
                setValue(raw_value);
            }

        """

        out += self.gen_set_value()
        out += "\n"
        out += self.gen_get_value()
        out += "\n"

        out += self.gen_fields_set_get()

        out += "};\n\n"

        out += "using %s = CSRValue<%s, CSRIdx::%s>;\n\n" % (self.name, xlen_str, self.idx_name)

        return out


def gen_file_open() -> str:
    return """
        #ifndef INCL_VALUE_GEN_HPP
        #define INCL_VALUE_GEN_HPP

        #include <sim/common.hpp>

        #include <sim/csr/idx.gen.hpp>
        #include <sim/csr/raw_value.hpp>

        namespace sim::csr {

        template <XLen xlen, CSRIdx idx> class CSRValue;

    """

def gen_csr_code(csr_name: str, csr_descr: dict) -> str:
    out = ""

    csr_name = csr_name.upper()

    xlen32_descr: dict = csr_descr.get("xlen32", None)
    xlen64_descr: dict = csr_descr.get("xlen64", None)

    mangle_xlen: bool = csr_descr.get("mangle_xlen", True)

    xlen32_name = csr_name + ("32" if mangle_xlen else "")
    xlen64_name = csr_name + ("64" if mangle_xlen else "")

    if xlen32_descr != None:
        out += CSRValue(xlen32_name, csr_name, 32, xlen32_descr).gen_code()

    if xlen64_descr != None:
        out += CSRValue(xlen64_name, csr_name, 64, xlen64_descr).gen_code()

    return out

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

    out = gen_file_open()

    for csr_name, csr_descr in csrs.items():
        out += gen_csr_code(csr_name, csr_descr)

    out += gen_file_close()

    with open(OUT_NAME, 'w') as out_file:
        out_file.write(out)

    os.system("clang-format -i %s" % OUT_NAME)

if __name__ == "__main__":
    main()
