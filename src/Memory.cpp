#include "../headers/Memory.h"


Memory::Memory(Config& config): config(config) {
    size = this->config.getMemSize();
    memory.resize(size);
}

void Memory::write(void *data, uint64_t addr, uint64_t bytes) {
    if (addr + bytes > size) {
        throw std::runtime_error("addr is out of memory");
    }
    std::memcpy(&memory[addr], data, bytes);
}

void Memory::read(void *data, uint64_t addr, uint64_t bytes) {
    if (addr + bytes > size) {
        throw std::runtime_error("addr is out of memory");
    }
    std::memcpy(data, &memory[addr], bytes);
}