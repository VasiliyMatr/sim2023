#include <sim/elf_load.hpp>

namespace sim::elf {

ElfLoader::LoadElfRes ElfLoader::loadElf(const char *elf_name) {
    // Check LibElf version
    SIM_ASSERT(elf_version(EV_CURRENT) != EV_NONE);

    // Open elf file
    int elf_file = open(elf_name, O_RDONLY);
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

            // Map pages
            VirtAddr seg_base = seg_vaddr & ~memory::PAGE_OFFSET_MASK;
            VirtAddr seg_end = seg_vaddr + seg_header.p_memsz;
            for (auto curr_page_va = seg_base; curr_page_va < seg_end;
                 curr_page_va += memory::PAGE_SIZE) {

                auto status = mapPage(curr_page_va / memory::PAGE_SIZE);
                if (status != SimStatus::OK) {
                    return {status, 0};
                }
            }

            // Load pages
            for (size_t offset = 0, end = seg_header.p_filesz; offset < end;) {
                VirtAddr curr_va = seg_vaddr + offset;
                size_t page_offset = curr_va & memory::PAGE_OFFSET_MASK;
                VirtAddr curr_page_va = curr_va - page_offset;

                auto it = m_mapping.find(curr_page_va / memory::PAGE_SIZE);
                SIM_ASSERT(it != m_mapping.end());
                PhysAddr curr_page_pa = it->second * memory::PAGE_SIZE;

                auto [status, host_page_ptr] = m_pm.write(curr_page_pa, 0);
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

    return {SimStatus::OK, elf_header.e_entry};
}

} // namespace sim::elf
