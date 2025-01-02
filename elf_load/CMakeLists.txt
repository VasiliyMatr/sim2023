# Describe elf module build

add_sim_module(elf_load)

find_package(LibElf REQUIRED)

target_sources(elf_load PRIVATE src/elf_load.cpp)

target_link_libraries(elf_load
PRIVATE
    ${LIBELF_LIBRARIES}
    sim::common
    sim::memory
)

target_include_directories(elf_load PUBLIC ${LIBELF_INCLUDE_DIRS})
