# Describe csr module build

add_sim_module(csr)

target_link_libraries(csr PUBLIC sim::common)

target_sources(csr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/csr.cpp)

# Codegen variables
set(CSR_YAML ${CMAKE_CURRENT_SOURCE_DIR}/csr.yaml)
set(CSR_INCLUDE_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/sim/csr)

file(MAKE_DIRECTORY ${CSR_INCLUDE_GEN_DIR})

# Generate idx.gen.hpp
add_custom_command(
    OUTPUT ${CSR_INCLUDE_GEN_DIR}/idx.gen.hpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_csr_idx.py ${CSR_YAML} ${CSR_INCLUDE_GEN_DIR}/idx.gen.hpp
    DEPENDS ${CSR_YAML} ${CMAKE_CURRENT_SOURCE_DIR}/gen_csr_idx.py
)

# Generate value.gen.hpp
add_custom_command(
    OUTPUT ${CSR_INCLUDE_GEN_DIR}/value.gen.hpp
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen_csr_value.py ${CSR_YAML} ${CSR_INCLUDE_GEN_DIR}/value.gen.hpp
    DEPENDS ${CSR_YAML} ${CMAKE_CURRENT_SOURCE_DIR}/gen_csr_value.py
)

# Add generated files deps
add_custom_target(csr_gen
DEPENDS
    ${CSR_INCLUDE_GEN_DIR}/idx.gen.hpp
    ${CSR_INCLUDE_GEN_DIR}/value.gen.hpp
)
add_dependencies(csr csr_gen)

# Add include path to generated headers
target_include_directories(csr PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
