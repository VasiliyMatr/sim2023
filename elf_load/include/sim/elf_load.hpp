#ifndef INCL_SIM_ELF_LOAD_HPP
#define INCL_SIM_ELF_LOAD_HPP

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <gelf.h>
#include <libelf.h>
#include <unistd.h>

#include <fcntl.h>
#include <sysexits.h>

#include <sim/common.hpp>
#include <sim/memory.hpp>

namespace sim::elf {

auto load(const std::string &filename, sim::memory::PhysMemory &pm) {
    SIM_ASSERT(elf_version(EV_CURRENT) != EV_NONE);

    // Open elf file
    int elf_file = open(filename.c_str(), O_RDONLY);
    SIM_ASSERT(elf_file != -1);

    // Get elf file size
    auto elf_size = lseek(elf_file, 0, SEEK_END);
    lseek(elf_file, 0, SEEK_SET);

    // Read elf file to buff
    std::vector<uint8_t> elf_buf(elf_size, 0);
    int read_num = read(elf_file, elf_buf.data(), elf_size);
    SIM_ASSERT(read_num != -1);

    Elf *elf = elf_begin(elf_file, ELF_C_READ, nullptr);
    SIM_ASSERT(elf != nullptr);
    SIM_ASSERT(gelf_getclass(elf) != ELFCLASSNONE);
    SIM_ASSERT(gelf_getclass(elf) != ELFCLASS32);

    GElf_Ehdr elf_header{};
    SIM_ASSERT(gelf_getehdr(elf, &elf_header) != nullptr);

    for (size_t i = 0; i < elf_header.e_phnum; ++i) {
        GElf_Phdr seg_header{};
        SIM_ASSERT(gelf_getphdr(elf, i, &seg_header) != nullptr);

        if (seg_header.p_type == PT_LOAD) {
            Elf64_Xword seg_vaddr = seg_header.p_vaddr;
            auto *seg_file_ptr = elf_buf.data() + seg_header.p_offset;

            // Add RAM pages
            for (VirtAddr curr_page_va = seg_vaddr & ~memory::PAGE_OFFSET_MASK,
                          end = seg_vaddr + seg_header.p_memsz;
                 curr_page_va < end; curr_page_va += memory::PAGE_SIZE) {
                SIM_ASSERT(pm.addRAMPage(curr_page_va));
            }

            // Fill RAM pages
            for (size_t offset = 0, end = seg_header.p_filesz; offset < end;) {
                VirtAddr curr_va = seg_vaddr + offset;
                size_t page_offset = curr_va & memory::PAGE_OFFSET_MASK;
                VirtAddr curr_page_va = curr_va - page_offset;

                auto [status, host_page_ptr] = pm.write(curr_page_va, 0);
                SIM_ASSERT(status == SimStatus::OK);
                SIM_ASSERT(host_page_ptr != nullptr);

                auto *host_begin = host_page_ptr + page_offset;
                size_t cpy_size =
                    std::min(memory::PAGE_SIZE - page_offset, end - offset);
                std::memcpy(host_begin, seg_file_ptr + offset, cpy_size);

                offset += cpy_size;
            }
        }
    }

    elf_end(elf);
    close(elf_file);

    return elf_header.e_entry;
}

constexpr memory::VPN DEFAULT_SP_VPN = 0xfffffff;
constexpr memory::VPN DEFAULT_STACK_VPN_SIZE = 0x1000;

auto map_stack(sim::memory::PhysMemory &pm, memory::VPN sp_vpn = DEFAULT_SP_VPN,
               size_t stack_vpn_size = DEFAULT_STACK_VPN_SIZE) {

    VirtAddr curr_va = (sp_vpn - stack_vpn_size) * memory::PAGE_SIZE;
    VirtAddr sp_va = sp_vpn * memory::PAGE_SIZE;

    for (; curr_va <= sp_va; curr_va += memory::PAGE_SIZE) {

        SIM_ASSERT(pm.addRAMPage(curr_va));
    }

    return sp_va;
}

} // namespace sim::elf

#endif // INCL_SIM_ELF_LOAD_HPP
