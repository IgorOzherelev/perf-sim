#ifndef _TYPES_H
#define _TYPES_H

enum AccessType {
    ACCESS_TYPE_READ,
    ACCESS_TYPE_WRITE,
    ACCESS_TYPE_NONE
};

enum InstrType {
    INSTR_TYPE_R,
    INSTR_TYPE_I,
    INSTR_TYPE_S,
    INSTR_TYPE_B,
    INSTR_TYPE_U,
    INSTR_TYPE_J,
    INSTR_TYPE_SYSTEM,
    INSTR_TYPE_NONE
};

enum InstrName {
    INSTR_NAME_LUI,
    INSTR_NAME_AUIPC,
    INSTR_NAME_JAL,
    INSTR_NAME_JALR,
    INSTR_NAME_BEQ,
    INSTR_NAME_BNE,
    INSTR_NAME_BLT,
    INSTR_NAME_BGE,
    INSTR_NAME_BLTU,
    INSTR_NAME_BGEU,
    INSTR_NAME_LB,
    INSTR_NAME_LH,
    INSTR_NAME_LW,
    INSTR_NAME_LBU,
    INSTR_NAME_LHU,
    INSTR_NAME_SB,
    INSTR_NAME_SH,
    INSTR_NAME_SW,
    INSTR_NAME_ADDI,
    INSTR_NAME_SLTI,
    INSTR_NAME_SLTIU,
    INSTR_NAME_XORI,
    INSTR_NAME_ORI,
    INSTR_NAME_ANDI,
    INSTR_NAME_SLLI,
    INSTR_NAME_SRLI,
    INSTR_NAME_SRAI,
    INSTR_NAME_ADD,
    INSTR_NAME_SUB,
    INSTR_NAME_SLL,
    INSTR_NAME_SLT,
    INSTR_NAME_SLTU,
    INSTR_NAME_XOR,
    INSTR_NAME_SRL,
    INSTR_NAME_SRA,
    INSTR_NAME_OR,
    INSTR_NAME_AND,
    INSTR_NAME_FENCE,
    INSTR_NAME_FENCE_I,
    INSTR_NAME_ECALL,
    INSTR_NAME_EBREAK,
    INSTR_NAME_CSRRW,
    INSTR_NAME_CSRRS,
    INSTR_NAME_CSRRC,
    INSTR_NAME_CSRRWI,
    INSTR_NAME_CSRRSI,
    INSTR_NAME_CSRRCI,
    INSTR_NAME_NONE,

};

enum RegName {
    REGISTER_NAME_ZERO = 0,
    REGISTER_NAME_RA,
    REGISTER_NAME_SP,
    REGISTER_NAME_GP,
    REGISTER_NAME_TP,
    REGISTER_NAME_T0,
    REGISTER_NAME_T1,
    REGISTER_NAME_T2,
    REGISTER_NAME_S0,
    REGISTER_NAME_S1,
    REGISTER_NAME_A0,
    REGISTER_NAME_A1,
    REGISTER_NAME_A2,
    REGISTER_NAME_A3,
    REGISTER_NAME_A4,
    REGISTER_NAME_A5,
    REGISTER_NAME_A6,
    REGISTER_NAME_A7,
    REGISTER_NAME_S2,
    REGISTER_NAME_S3,
    REGISTER_NAME_S4,
    REGISTER_NAME_S5,
    REGISTER_NAME_S6,
    REGISTER_NAME_S7,
    REGISTER_NAME_S8,
    REGISTER_NAME_S9,
    REGISTER_NAME_S10,
    REGISTER_NAME_S11,
    REGISTER_NAME_T3,
    REGISTER_NAME_T4,
    REGISTER_NAME_T5,
    REGISTER_NAME_T6,
    REGISTER_NAME_NONE
};

#endif //_TYPES_H
