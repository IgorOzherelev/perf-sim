#ifndef _Instr_H
#define _Instr_H

#include <cstdint>
#include <iostream>
#include "Stage.h"
#include "types.h"
#include "Executors.h"

class Register {
private:
    int32_t value = 0;
public:
    int32_t get_value() const { return value; }
    void set_value(int32_t val){value = val;}
    RegName get_name() {return name;}
    Register() = default;
    ~Register() = default;

    RegName name;
};

class Instruction {
    InstrType type = INSTR_TYPE_NONE;
    InstrName name = INSTR_NAME_NONE;
    uint32_t opcode{};
    uint32_t pc{};
    AccessType mem_acc_type = ACCESS_TYPE_NONE;
    bool is_branch = false;
    uint32_t branch_addr = 0;
    void(*main_executor)(Instruction*) = nullptr;
    friend class Decoder;
public:
    uint32_t get_pc() const{ return pc;}
    void set_pc(uint32_t pc_) { this->pc =pc_;}
    AccessType get_mem_acc_type(){return mem_acc_type;}
    bool get_is_branch() const{return is_branch;}
    uint32_t get_br_target_addr() const{  if (is_branch) return branch_addr; else  throw  1;};
    void set_br_target_addr(uint32_t addr) {branch_addr = addr; if (!is_branch ) throw 1;};
    
    Instruction() = default;
    Instruction(InstrName name, InstrType type);
    virtual void calc_imm(uint32_t instr);
    virtual uint32_t get_imm() const{throw 1;}
    virtual uint32_t get_f3() const{ throw 1; }
    virtual uint32_t get_f7() const{ throw 1; }
    virtual Register get_rs1() const{ throw 1; };
    virtual Register get_rs2() const{ throw 1; };
    virtual Register get_rd() const{throw 1; };
    virtual Register& get_rd_ref() { throw 1; };
    void execute();
    InstrName get_name() const{ return name;}
    InstrType get_type() const{ return type;}
    virtual ~Instruction() = default ;
};

class InstructionR : public Instruction {
private:
    uint32_t funct3{};
    uint32_t funct7{};
    Register rs1;
    Register rs2;
    Register rd;
    friend class Decoder;
public:
    uint32_t get_f3() const override{return funct3;}
    uint32_t get_f7() const override{return funct7;}
    Register get_rs1() const override{return rs1;};
    Register get_rs2() const override{return rs2;};
    Register get_rd() const override{ return rd; };
    Register& get_rd_ref() override { return rd; };
    explicit InstructionR(InstrName name);
    InstructionR()= default;
    ~InstructionR()  override = default;

};

class InstructionI: public Instruction {
private:
    uint32_t funct3{};
    uint32_t imm{};
    Register rs1;
    Register rd;
    int32_t load_addres{};
    int32_t load_size{};
    friend class Decoder;
public:
    void calc_imm(uint32_t instr) override;
    uint32_t get_imm() const override{return imm;}
    uint32_t get_f3() const override{ return funct3; }
    Register get_rs1() const override{ return rs1; };
    Register get_rd() const override{ return rd; };
    Register& get_rd_ref() override { return rd; };
    int32_t get_load_addr() const{ return load_addres;}
    int32_t get_load_size() const{ return load_size;}
    void set_load_addr(int32_t l_add) {load_addres =l_add;}
    explicit InstructionI(InstrName name);
    InstructionI()= default;
    ~InstructionI() override = default;
};

class InstructionS: public Instruction {
private:
    uint32_t funct3{};
    uint32_t imm{};
    Register rs1;
    Register rs2;
    int32_t store_addres{};
    int32_t store_size{};
    friend class Decoder;
public:
    void calc_imm(uint32_t instr) override;
    uint32_t get_imm() const override { return imm; }
    uint32_t get_f3() const override{ return funct3; }
    Register get_rs1() const override{ return rs1; }
    Register get_rs2() const override{ return rs2; }
    int32_t get_store_addr() const{ return store_addres;}
    int32_t get_store_size() const { return store_size;}
    void set_store_addr(int32_t sa){store_addres=sa;}
    explicit InstructionS(InstrName name);
    InstructionS()= default;
    ~InstructionS() override = default;
};

class InstructionB: public Instruction {
private:
    uint32_t funct3{};
    uint32_t imm{};
    Register rs1;
    Register rs2;
    bool br_is_taken =false;
    friend class Decoder;
    friend void Executors::BGE(Instruction *instr );
    friend void Executors::BGEU(Instruction *instr );
    friend void Executors::BEQ(Instruction *instr );
    friend void Executors::BNE(Instruction *instr );
    friend void Executors::BLT(Instruction *instr );
    friend void Executors::BLTU(Instruction *instr );
    
public:
    void calc_imm(uint32_t instr) override;
    uint32_t get_imm() const override { return imm; }
    uint32_t get_f3() const override { return funct3; }
    Register get_rs1() const override { return rs1; }
    Register get_rs2() const override { return rs2; }
    bool get_br_is_taken() const{return br_is_taken;}
    explicit InstructionB(InstrName name);
    InstructionB()= default;
    ~InstructionB() override = default;
};

class InstructionU: public Instruction {
private:
    uint32_t imm{};
    Register rd;
    friend class Decoder;
public:
    void calc_imm(uint32_t instr) override;
    Register get_rd() const override{ return rd; };
    Register& get_rd_ref() override { return rd; };
    uint32_t get_imm() const override{ return imm; }
    explicit InstructionU(InstrName name);
    InstructionU()= default;
    ~InstructionU() override = default;
};

class InstructionJ: public Instruction {
private:
    uint32_t imm{};
    Register rd;
    friend class Decoder;
public:
    void calc_imm(uint32_t instr) override;
    uint32_t get_imm() const override{ return imm; }
    Register get_rd() const override{ return rd; };
    Register& get_rd_ref() override { return rd; };
    explicit InstructionJ(InstrName name);
    InstructionJ()= default;
    ~InstructionJ() override = default;
};

#endif