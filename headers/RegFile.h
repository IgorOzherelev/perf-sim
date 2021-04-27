#ifndef _REGFILE_H
#define _REGFILE_H

#include "Instruction.h"
#include <vector>
#include "Config.h"


class RegFile {
private:
    uint32_t num_regs = 0;
    std::vector<Register> regs;
    std::vector<int32_t> reg_locker;
    Config& config;
public:
    Register getReg(RegName num, AccessType acc = ACCESS_TYPE_READ);
    int32_t is_lock(uint32_t num){return reg_locker[num];}
    void write(Register reg);
    explicit RegFile(Config& config);
    ~RegFile() = default;
};

#endif //_REGFILE_H
