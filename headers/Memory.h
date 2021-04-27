#ifndef _MEMORY_H
#define _MEMORY_H
#include <cstdint>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstring>

#include "Config.h"
#include "Log.h"

class Memory {
private:
    Config& config;
    uint64_t size = 0;
    std::vector<char> memory;
public:
    explicit Memory(Config& config);
    ~Memory() = default;
    void read(void* data, uint64_t addr, uint64_t bytes);
    void write(void* data, uint64_t addr, uint64_t bytes);
};

#endif //_MEMORY_H
