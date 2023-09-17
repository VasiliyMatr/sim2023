
gen = open("decode.gen.cpp", "w")

gen.write(
    "#include <sim/instr.hpp>" "\n"
    "\n"
    "namespace sim {" "\n"
    "namespace instr {" "\n"
    "\n"
    "Instr::Instr(InstrCode instr_code) {}" "\n"
    "\n"
    "}" "\n"
    "}" "\n"
)
