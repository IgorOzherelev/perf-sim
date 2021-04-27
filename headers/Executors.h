#ifndef _EXECUTORS_H
#define _EXECUTORS_H

namespace Executors {
    void LUI(Instruction *instr);
    void AUIPC(Instruction *instr);
    void JAL(Instruction *instr);
    void JALR(Instruction *instr);
    void BEQ(Instruction *instr);
    void BNE(Instruction *instr);
    void BLT(Instruction *instr);
    void BGE(Instruction *instr);
    void BLTU(Instruction *instr);
    void BGEU(Instruction *instr);
    void Store(Instruction *instr);
    void Load(Instruction *instr);
    void ADDI(Instruction *instr);
    void SLTI(Instruction *instr);
    void SLTIU(Instruction *instr);
    void XORI(Instruction *instr);
    void ORI(Instruction *instr);
    void ANDI(Instruction *instr);
    void SLLI(Instruction *instr);
    void SRLI(Instruction *instr);
    void SRAI(Instruction *instr);
    void ADD(Instruction *instr);
    void SUB(Instruction *instr);
    void SLL(Instruction *instr);
    void SLT(Instruction *instr);
    void SLTU(Instruction *instr);
    void XOR(Instruction *instr);
    void SRL(Instruction *instr);
    void SRA(Instruction *instr);
    void OR(Instruction *instr);
    void AND(Instruction *instr);
    void ECALL(Instruction * instr);
}

#endif //_EXECUTORS_H
