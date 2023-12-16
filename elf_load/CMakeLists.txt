# Describe elf module build

add_sim_header_module(elf_load)

find_package(LibElf REQUIRED)

target_link_libraries(elf_load
INTERFACE
    ${LIBELF_LIBRARIES}
    sim::common
    sim::memory
)

target_include_directories(elf_load INTERFACE ${LIBELF_INCLUDE_DIRS})
