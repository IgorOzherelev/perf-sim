#include "../headers/Instruction.h"

void Executors::LUI(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionU *>(instr);
    Register& rd = instrT->get_rd_ref();
    uint32_t imm = instrT->get_imm();
    uint32_t val = imm ;
    rd.set_value(val);
}

void Executors::AUIPC(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionU *>(instr);
    Register& rd = instrT->get_rd_ref();
    int32_t imm = instrT->get_imm();
    int32_t val = imm + instr->get_pc();
    rd.set_value(val);
}

void Executors::Store(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionS*>(instr);
    Register rs1 = instrT->get_rs1();
    int32_t imm = instrT->get_imm();
    int32_t addr = imm + rs1.get_value();
    instrT->set_store_addr(addr);
}

void Executors::Load(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionI*>(instr);
    Register rs1 = instrT->get_rs1();
    int32_t imm = instrT->get_imm();
    int32_t addr = imm + rs1.get_value();
    instrT->set_load_addr(addr);
    
}

void Executors::ADDI(Instruction *instr) {
    InstructionI * instrT = dynamic_cast<InstructionI *>(instr);
    int32_t imm = instrT->get_imm();
    Register rs1 = instrT->get_rs1();
    Register& rd = instrT->get_rd_ref();
    int64_t res = static_cast<int64_t>(rs1.get_value()) + imm;
    rd.set_value(static_cast<int32_t>(res));
}

void Executors::SLTI(Instruction *instr) {
    int32_t imm = instr->get_imm();
    Register rs1 = instr->get_rs1();
    Register& rd = instr->get_rd_ref();
    int32_t res;
    if (static_cast<int32_t>(rs1.get_value()) < static_cast<int32_t>(imm)) {
        res = 1;
    } else {
        res = 0;
    }
    rd.set_value(static_cast<int32_t>(res));
}

void Executors::SLTIU(Instruction *instr) {
    int32_t imm = instr->get_imm();
    Register rs1 = instr->get_rs1();
    Register& rd = instr->get_rd_ref();
    int32_t res;
    if (static_cast<uint32_t>(rs1.get_value()) < static_cast<uint32_t>(imm)) {
        res = 1;
    } else {
        res = 0;
    }
    rd.set_value(static_cast<int32_t>(res));
}

void Executors::XORI(Instruction *instr) {
    int32_t imm = instr->get_imm();
    Register rs1 = instr->get_rs1();
    Register& rd = instr->get_rd_ref();
    int32_t res;
    res = rs1.get_value() & imm;
    rd.set_value(res);
}

void Executors::ORI(Instruction *instr) {
    int32_t imm = instr->get_imm();
    Register rs1 = instr->get_rs1();
    Register& rd = instr->get_rd_ref();
    int32_t res;
    res = rs1.get_value() | imm;
    rd.set_value(res);
}

void Executors::ANDI(Instruction *instr) {
    int32_t imm = instr->get_imm();
    Register rs1 = instr->get_rs1();
    Register& rd = instr->get_rd_ref();
    int32_t res;
    res = rs1.get_value() ^ imm;
    rd.set_value(res);
}

void Executors::SLLI(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionI *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    uint32_t imm = instrT->get_imm();
    uint32_t shift = imm & 0b11111;
    int32_t res;
    res = rs1.get_value() << shift;
    rd.set_value(res);
}

void Executors::SRLI(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionI *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    uint32_t imm = instrT->get_imm();
    uint32_t shift = imm & 0b11111;
    int32_t res;
    res = static_cast<uint32_t >(rs1.get_value()) >> shift;
    rd.set_value(res);
}

void Executors::SRAI(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionI *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    uint32_t imm = instrT->get_imm();
    uint32_t shift = imm & 0b11111;
    int32_t res;
    res = rs1.get_value() >> shift;
    rd.set_value(res);
}

void Executors::ADD(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res = rs1.get_value() + rs2.get_value();
    rd.set_value(res);
}

void Executors::SUB(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res = rs1.get_value() - rs2.get_value();
    rd.set_value(res);
}

void Executors::SLL(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res = rs1.get_value() << rs2.get_value();
    rd.set_value(res);
}

void Executors::SLT(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    uint32_t res;
    if(static_cast<int32_t>(rs1.get_value()) < static_cast<int32_t>(rs2.get_value())) {
        res = 1;
    } else {
        res = 0;
    }
    rd.set_value(static_cast<uint32_t>(res));
}

void Executors::SLTU(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    uint32_t res;
    if(static_cast<uint32_t>(rs1.get_value()) < static_cast<uint32_t>(rs2.get_value())) {
        res = 1;
    } else {
        res = 0;
    }
    rd.set_value(static_cast<uint32_t>(res));
}

void Executors::XOR(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res;
    res = rs1.get_value() ^ rs2.get_value();
    rd.set_value(res);
}

void Executors::SRL(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res = static_cast<uint32_t >(rs1.get_value()) >> rs2.get_value();
    rd.set_value(res);
}

void Executors::SRA(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res = rs1.get_value() >> rs2.get_value();
    rd.set_value(res);
}
void Executors::OR(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res;
    res = rs1.get_value() | rs2.get_value();
    rd.set_value(res);
}

void Executors::AND(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionR *>(instr);
    Register& rd = instrT->get_rd_ref();
    Register rs1 = instr->get_rs1();
    Register rs2 = instr->get_rs2();
    int32_t res;
    res = rs1.get_value() & rs2.get_value();
    rd.set_value(res);
}

void Executors::BLTU(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionB *>(instr);
    auto  rs1_val = static_cast<uint32_t >(instrT->rs1.get_value());
    auto  rs2_val = static_cast<uint32_t >(instrT->rs2.get_value());
    bool res = false;
    if (rs1_val < rs2_val)
    {
        res = true;
    }
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    instrT->br_is_taken = res;
}

void Executors::BGEU(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionB *>(instr);
    auto  rs1_val = static_cast<uint32_t >(instrT->rs1.get_value());
    auto  rs2_val = static_cast<uint32_t >(instrT->rs2.get_value());
    bool res = false;
    if (rs1_val >= rs2_val) {
        res = true;
    }
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    instrT->br_is_taken = res;
}

void Executors::BEQ(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionB *>(instr);
    auto  rs1_val = static_cast<uint32_t >(instrT->rs1.get_value());
    auto  rs2_val = static_cast<uint32_t >(instrT->rs2.get_value());
    bool res = false;
    if (rs1_val == rs2_val) {
        res = true;
    }
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    instrT->br_is_taken = res;
}

void Executors::BNE(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionB *>(instr);
    auto  rs1_val = static_cast<uint32_t >(instrT->rs1.get_value());
    auto  rs2_val = static_cast<uint32_t >(instrT->rs2.get_value());
    bool res = false;
    if (rs1_val != rs2_val)
    {
        res = true;
    }
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    instrT->br_is_taken = res;
}

void Executors::BLT(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionB *>(instr);
    auto  rs1_val = static_cast<int32_t >(instrT->rs1.get_value());
    auto  rs2_val = static_cast<int32_t >(instrT->rs2.get_value());
    bool res = false;
    if (rs1_val < rs2_val) {
        res = true;
    }
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    instrT->br_is_taken = res;
}

void Executors::BGE(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionB *>(instr);
    auto  rs1_val = static_cast<int32_t >(instrT->rs1.get_value());
    auto  rs2_val = static_cast<int32_t >(instrT->rs2.get_value());
    bool res = false;
    if (rs1_val >= rs2_val)
    {
        res = true;
    }
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    instrT->br_is_taken = res;
}

void Executors::JAL(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionJ*>(instr);
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+instr->get_pc();
    instrT-> set_br_target_addr(j_addr);
    
    Register& rd = instrT->get_rd_ref();
    rd.set_value(instr->get_pc()+4);
}

void Executors::JALR(Instruction *instr) {
    auto *instrT = dynamic_cast<InstructionI*>(instr);
    Register rs1 = instrT->get_rs1();
    int32_t j_addr = static_cast<int32_t>(instrT->get_imm())+rs1.get_value();
    instrT-> set_br_target_addr(j_addr);
    
    Register& rd = instrT->get_rd_ref();
    rd.set_value(instr->get_pc()+4);
}

void Executors::ECALL(Instruction *instr) {
    (void)instr;
}