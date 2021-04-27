#ifndef _READ_ELF_H
#define _READ_ELF_H

#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <vector>

#include"Memory.h"


class ElfManager {
    uint32_t entry = 0;
    Elf *e;
    int fd;
public:
    ElfManager(const char * str);
    ~ElfManager();
    uint32_t getEntry() const { return entry;}

    void load(Memory &memory);
};
#endif //_READ_ELF_H
