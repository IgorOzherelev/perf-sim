#ifndef _STAGE_H
#define _STAGE_H

#include <stdexcept>

enum PipelineStage {
    PIPELINE_STAGE_EXECUTE,
    PIPELINE_STAGE_MEMORY,
};

class Instruction;

struct WF {
    bool is_stall = false;
    bool hazard_stall = false;
    bool is_jump = false;
    uint32_t pc = 0;
    uint32_t pc_jump = 0;
    void clear() {
        is_stall = false;
        hazard_stall = false;
        is_jump = false;
        pc = 0;
        pc_jump = 0;
    }
};

struct FD {
    bool is_stall = true;
    uint32_t pc = 0;
    uint32_t instr = 0;
    bool hazard_stall = false;
    void clear() {
        is_stall =true;
        hazard_stall = false;
        pc = 0;
        instr = 0;
    }
};

struct DE {
    bool is_stall = true;
    bool hazard_stall = false;
    uint32_t pc = 0;
    Instruction * instr = nullptr;
    void clear() {
        is_stall =true;
        hazard_stall = false;
        pc = 0;
        instr = nullptr;
    }
};

struct EM {
    bool is_stall = true;
    bool hazard_stall = false;
    uint32_t pc = 0;
    Instruction * instr = nullptr;
    void clear() {
        is_stall =true;
        hazard_stall = false;
        pc = 0;
        instr = nullptr;
    }
};

struct MW {
    bool is_stall = true;
    bool hazard_stall = false;
    uint32_t pc = 0;
    Instruction* inst = nullptr;
    void clear() {
        is_stall =true;
        hazard_stall = false;
        pc = 0;
        inst = nullptr;
    }
};

template <typename T>
class Stage {
public:
    T* store;
    T* load;
    uint32_t stop_count = 0;

    void update();
    T* getStorePtr(){return store;}
    T* getLoadPtr(){return load;}
    void extend(uint32_t count){ stop_count += count;}
    bool is_stop(){return (stop_count > 0);}
    Stage();
    ~Stage();
};

template<typename T>
Stage<T>::Stage() {
    store = new T;
    load = new T;
    if (!store || !load)
        throw std::runtime_error("allocation error");
}

template<typename T>
Stage<T>::~Stage() {
    delete store;
    delete load;
}

template<typename T>
void Stage<T>::update() {
    T* temp;
    if (stop_count > 0) {
        stop_count--;
        return;
    }
    temp = store;
    store = load;
    store->clear();
    load = temp;
}

#endif //_STAGE_H
