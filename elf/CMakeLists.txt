# Describe elf module build

add_sim_header_module(elf)

find_package(LibElf REQUIRED)

target_link_libraries(elf
INTERFACE
    ${LIBELF_LIBRARIES}
    elf
    sim::common
    sim::memory
)

target_include_directories(elf INTERFACE ${LIBELF_INCLUDE_DIRS})