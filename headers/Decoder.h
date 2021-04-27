#ifndef DECODER_H
#define DECODER_H

#include "Instruction.h"
#include "Log.h"
#include "Config.h"
#include "HazardUnit.h"
#include <vector>

class RegFile;

template <typename T>
class CyclicArray {
private:
    uint64_t size = 0;
    uint64_t index = 0;
    std::vector<T> cont;
public:
    explicit CyclicArray(uint32_t size) : size(size) { cont.resize(size); };
    ~CyclicArray() = default;
    T* get_next(){return &cont[index++%size]; }
};

class Decoder {
private:
    Config& config;
    HazardUnit& hu;
    uint32_t pipeline_size;
    InstrName name = INSTR_NAME_NONE;
    InstrType type = INSTR_TYPE_NONE;
    AccessType mem_acc_type = ACCESS_TYPE_NONE;
    int32_t acc_size;
    bool is_branch;
    CyclicArray<InstructionI> i_instr_arr;
    CyclicArray<InstructionB> b_instr_arr;
    CyclicArray<InstructionR> r_instr_arr;
    CyclicArray<InstructionS> s_instr_arr;
    CyclicArray<InstructionU> u_instr_arr;
    CyclicArray<InstructionJ> j_instr_arr;
    CyclicArray<Instruction> sys_instr_arr;
    void (*executor)(Instruction*) =nullptr;
    uint32_t instr;
    void recognize(uint32_t opcode, uint32_t funct3, uint32_t funct7);
public:
    explicit Decoder(Config& config, HazardUnit& hz);
    ~Decoder() = default;
    Instruction* decode(uint32_t instr_, RegFile& reg);
};

#endif // !DECODER_H
