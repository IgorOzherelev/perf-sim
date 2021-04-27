#ifndef PIPELINE_H
#define PIPELINE_H

#include "Instruction.h"
#include "Memory.h"
#include "Config.h"
#include <list>


class Store {
public:
    uint64_t addr = 0;
    uint64_t size = 0;
    uint32_t data = 0;
    uint32_t id = 0;
    void clear() {
        addr = 0;
        size = 0;
        data = 0;
        id = 0;
    }
};

class Pipeline {
private:
    uint32_t id = 0;
    Memory& memory;
    Store store;
    std::list<Store> queue;
    Config& config;
    bool tryConcatenate(Store store);
public:
    Pipeline(Memory& memory, Config& config);
    ~Pipeline()= default;;
    void push(Instruction* instruction);
    void update();
    uint32_t searchData(Instruction* instruction);
};

#endif // !PIPELINE_H