#ifndef INCL_ELF_HPP
#define INCL_ELF_HPP

#include <stdlib.h>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <sysexits.h>

#include <string>
#include <cstdio>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim{
namespace elf{

auto load(const std::string& filename, sim::memory::PhysMemory& pm) {

    int elf_file = open(filename.c_str(), O_RDONLY);

    SIM_ASSERT(elf_file != -1);

    auto elf_size = lseek(elf_file, 0, SEEK_END);
    lseek(elf_file, 0, SEEK_SET);

    std::vector<uint8_t> elf_buf(elf_size);

    int read_error = read(elf_file, elf_buf.data(), elf_size);
    SIM_ASSERT(read_error != -1);

    SIM_ASSERT(elf_version(EV_CURRENT) != EV_NONE);
    Elf* the_elf = elf_begin(elf_file, ELF_C_READ, nullptr);
    SIM_ASSERT(the_elf != nullptr);

    GElf_Ehdr elf_header;
    SIM_ASSERT(gelf_getehdr(the_elf, &elf_header) != nullptr);

    auto entry_point = elf_header.e_entry;
    
    SIM_ASSERT(gelf_getclass(the_elf) != ELFCLASSNONE);
    SIM_ASSERT(gelf_getclass(the_elf) != ELFCLASS32);

    for (size_t i = 0; i < elf_header.e_phnum; ++i) {
        GElf_Phdr seg_header;
        SIM_ASSERT(gelf_getphdr(the_elf, i, &seg_header) != nullptr);

        if (seg_header.p_type == PT_LOAD){
            
            Elf64_Xword seg_vaddr = seg_header.p_vaddr;

            for (PhysAddr page_pa = static_cast<PhysAddr>(seg_vaddr), end = static_cast<PhysAddr>(seg_header.p_memsz) + static_cast<PhysAddr>(seg_vaddr);
                page_pa < end; page_pa += sim::memory::PAGE_SIZE) {
                SIM_ASSERT(pm.addRAMPage(page_pa));
            }

            for (size_t i = 0, end = seg_header.p_filesz; i != end; ++i) {
                SIM_ASSERT(
                    pm.write(seg_vaddr + i * sizeof(uint8_t), elf_buf[seg_header.p_offset + i])
                        .status == sim::memory::PhysMemory::AccessStatus::OK);
            }
        }
    }

    elf_end(the_elf);
    close(elf_file);

    return entry_point;
}

} //namespace elf
} //namespace sim

#endif // INCL_ELF_HPP