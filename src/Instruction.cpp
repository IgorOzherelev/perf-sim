#include "../headers/Instruction.h"


Instruction::Instruction(InstrName name, InstrType type) :type(type), name(name){}
InstructionI::InstructionI(InstrName name) : Instruction(name, INSTR_TYPE_I){}
InstructionR::InstructionR(InstrName name) : Instruction(name, INSTR_TYPE_R){}
InstructionS::InstructionS(InstrName name) : Instruction(name, INSTR_TYPE_S){}
InstructionB::InstructionB(InstrName name) : Instruction(name, INSTR_TYPE_B){}
InstructionU::InstructionU(InstrName name) : Instruction(name, INSTR_TYPE_U){}
InstructionJ::InstructionJ(InstrName name) : Instruction(name, INSTR_TYPE_J){}

void Instruction::execute() {
    if (this->main_executor) {
        main_executor(this);
    }
}

void Instruction::calc_imm(uint32_t instr) {
    (void)instr;
}

void InstructionI::calc_imm(uint32_t instr) {
    uint32_t s1 = 0;
    if (((instr >> 31) & 0b1) == 0b1)
    {
        s1 = 0b111111111111111111111u<<11;
    }
    uint32_t s2 = 0;
    s2 = ((instr >> 25) & 0b111111) <<5;
    uint32_t s3 = 0;
    s3 = ((instr >>21) & 0b1111) <<1;
    uint32_t s4 = 0;
    s4 = (instr >>20) & 0b1;
    imm = s1 +s2 +s3 +s4;

}

void InstructionS::calc_imm(uint32_t instr) {
    uint32_t s1 = 0;
    if (((instr >> 31) & 0b1) == 0b1)
    {
        s1 = 0b111111111111111111111u << 11;
    }
    uint32_t s2 = 0;
    s2 = ((instr >> 25) & 0b111111) << 5;
    uint32_t s3 = 0;
    s3 = ((instr >> 8) & 0b1111) << 1;
    uint32_t s4 = 0;
    s4 = (instr >> 7) & 0b1;
    imm = s1 + s2 + s3 + s4;
}

void InstructionB::calc_imm(uint32_t instr) {
    uint32_t s1 = 0;
    if (((instr >> 31) & 0b1) == 0b1)
    {
        s1 = 0b11111111111111111111u << 12;
    }
    uint32_t s2 = 0;
    s2 = ((instr >> 25) & 0b111111) << 5;
    uint32_t s3 = 0;
    s3 = ((instr >> 8) & 0b1111) << 1;
    uint32_t s4 = 0;
    s4 = ((instr >> 7) & 0b1) <<11;
    imm = s1 + s2 + s3 + s4;

}

void InstructionU::calc_imm(uint32_t instr) {
    
    imm = instr & 0b11111111111111111111000000000000u;
}

void InstructionJ::calc_imm(uint32_t instr) {
    uint32_t s1 = 0;
    if (((instr >> 31) & 0b1) == 0b1)
    {
        s1 = 0b111111111111u << 20u;
    }
    uint32_t s2 = 0;
    s2 = instr & (0b11111111u <<12);
    uint32_t s3 = ((instr >> 20) & 0b1u) << 11;
    uint32_t s4 = ((instr >> 25) & 0b111111u) << 5;
    uint32_t s5 = 0;
    s5 = ((instr >> 21) & 0b1111) << 1;
    imm = s1 +s2 +s3 +s4 +s5;
}

