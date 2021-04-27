#include "../headers/RegFile.h"


RegFile::RegFile(Config& config) : config(config) {
    num_regs = static_cast<uint32_t >(this->config.getNumRegs());
    reg_locker.resize(num_regs, 0);
    regs.resize(num_regs);
    for (uint32_t i = 0; i < num_regs; i++)
    {
        regs[i].name = RegName(i);
    }
}

Register RegFile::getReg(RegName num, AccessType acc) {
    if (acc == ACCESS_TYPE_WRITE && REGISTER_NAME_ZERO!= num)
        reg_locker[num]--;
    return regs[num];
}

void RegFile::write(Register reg) {
    uint32_t num = reg.get_name();
    if (num > this->num_regs)
        throw std::runtime_error("num_regs error");
    if (REGISTER_NAME_ZERO == num)
        return;
    regs[num] = reg;
    reg_locker[num]++;
}