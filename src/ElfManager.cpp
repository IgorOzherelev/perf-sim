#include <iostream>
#include "../headers/ElfManager.h"

ElfManager::ElfManager(const char * path_to_elf) {
    int class_;
    GElf_Ehdr ehdr;
    
    if (elf_version( EV_CURRENT ) == EV_NONE)
        errx(EXIT_FAILURE, " ELF library initialization failed : %s ", elf_errmsg(-1));
    if ((fd = open (path_to_elf, O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, " open \"%s \" failed ", path_to_elf);
    if ((e = elf_begin(fd, ELF_C_READ , nullptr)) == nullptr)
        errx(EXIT_FAILURE, " elf_begin () failed : %s . ",
               elf_errmsg(-1));
    if (elf_kind (e) != ELF_K_ELF)
        errx(EXIT_FAILURE, " \"%s \" is not an ELF object . ", path_to_elf);
    if (gelf_getehdr(e, & ehdr) == nullptr)
        errx(EXIT_FAILURE, " getehdr () failed : %s . ",
           elf_errmsg(-1));
    if ((class_ = gelf_getclass(e)) == ELFCLASSNONE)
        errx (EXIT_FAILURE, " getclass () failed : %s . ",
           elf_errmsg ( -1));
    if (class_ == ELFCLASS32) {
        entry = static_cast<uint32_t>(ehdr.e_entry);
    }
}

void ElfManager::load(Memory &memory) {
    size_t phdrnum;
    elf_getphdrnum(e, &phdrnum);

    std::vector<char> buf(1024);
    GElf_Phdr phdr;
    for (int i = 0; static_cast<size_t>(i) < phdrnum; i++ ) {
        if (gelf_getphdr(e, i, &phdr) != &phdr) {
            throw std::runtime_error("error: can't get phdr");
        }

        if (buf.size() < phdr.p_filesz) {
            buf.resize(phdr.p_filesz);
        }

        if (phdr.p_offset != static_cast<uint64_t>(lseek(fd, (std::int64_t)phdr.p_offset, SEEK_SET))) {
            throw std::runtime_error("error: lseek failed");
        }
        ssize_t read_bytes = read(fd, &buf[0], phdr.p_filesz);
        if (read_bytes < 0 || phdr.p_filesz != static_cast<uint32_t>(read_bytes)) {
            throw std::runtime_error("error: read instr fail");
        }
        memory.write(&buf[0], phdr.p_vaddr, phdr.p_filesz);
    }
}

ElfManager::~ElfManager() {
    elf_end(e);
    close(fd);
}