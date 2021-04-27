#include "../headers/PerfSim.h"
#include <iostream>

PerfSim::PerfSim(const std::string& path_to_data): config(), log(config.getLogRef()),
                trace(config.getTraceRef()), memory(config), rf(config),
                hu(config, rf, fd_st, de_st, em_st, mw_st, fetch_st),
                decoder(config, hu), pipeline(memory, config), is_dump_trace(config.isDumpTrace()) {
    ElfManager elf(path_to_data.c_str());
    elf.load(memory);
    
    Register sp = rf.getReg(REGISTER_NAME_SP, ACCESS_TYPE_WRITE);
    sp.set_value(config.getMemSize() - 5);
    
    uint32_t argc = 0;
    memory.write(&argc, sp.get_value(), 4);
    rf.write(sp);
    
    WF* wf = fetch_st.getStorePtr();
    wf->pc = static_cast<uint32_t >(elf.getEntry());
    wf->is_stall = false;
    fetch_st.update();
}

void PerfSim::run() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    while(!is_stop) {
        writeBack();
        memoryAccess();
        execute();
        decode();
        fetch();
        
        fetch_st.update();
        fd_st.update();
        de_st.update();
        em_st.update();
        mw_st.update();
        pipeline.update();
        clocks++;
    }
    end = std::chrono::system_clock::now();
    
    execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>
        (end-start).count();

    if (this->config.getLog()) {
        dump();
    }
}

void PerfSim::fetch() {
    WF* wf = fetch_st.getLoadPtr();
    WF* wf_store = fetch_st.getStorePtr();
    FD* fd = fd_st.getStorePtr();
    uint32_t  instr;
    fd->is_stall =wf->is_stall || wf->hazard_stall;
    if (fd->is_stall || fd_st.is_stop()) {
        return;
    }
    uint32_t  pc = wf->is_jump? wf->pc_jump :wf->pc;
    memory.read(&instr, pc, 4);
    
    fd->pc = static_cast<uint32_t>(pc);
    fd->instr = instr;
    fd->is_stall = false;
    wf_store->pc = pc + 4;
}

void PerfSim::decode() {
    FD* fd = fd_st.getLoadPtr();
    DE* de = de_st.getStorePtr();
    de->is_stall = fd->is_stall || fd->hazard_stall;
    if (de->is_stall || de_st.is_stop()) {
        return;
    }
    Instruction* instr;
    instr = decoder.decode(fd->instr, rf);
    instr->set_pc(fd->pc);
    if (de_st.is_stop()) {
        hu.write(instr);
    }
    de->instr = instr;
    de->pc = fd->pc;
}

void PerfSim::execute() {
    DE* de = de_st.getLoadPtr();
    EM* em = em_st.getStorePtr();
    em->is_stall = de->is_stall || de->hazard_stall;
    Instruction* Instr = de->instr;
    if(em->is_stall || em_st.is_stop()) {
        if (!de->is_stall && de->hazard_stall)
            hu.write(Instr);
        return;
    }

    Instr->execute();
    if (Instr->get_is_branch() &&
        (INSTR_TYPE_J == Instr->get_type() ||
        INSTR_TYPE_I == Instr->get_type()  ||
            (INSTR_TYPE_B == Instr->get_type() && dynamic_cast<InstructionB *>(Instr)->get_br_is_taken()))) {
        hu.branch(Instr);
    }
    
    if (INSTR_TYPE_SYSTEM == Instr->get_type()) {
        hu.exit(Instr);
    }
    
    em->pc = de->pc;
    em->instr = de->instr;
    em->is_stall = de->is_stall;
}
void PerfSim::memoryAccess() {
    EM* em = em_st.getLoadPtr();
    MW* mw = mw_st.getStorePtr();
    Instruction* Instr = em->instr;
    mw->is_stall = em->is_stall || em->hazard_stall;
    if (mw->is_stall || mw_st.is_stop()) {
        if (!em->is_stall  && em->hazard_stall)
            hu.write(Instr);
        return;
    }
    AccessType instr_acc_type = Instr->get_mem_acc_type();
    InstrType  instr_type = Instr->get_type();
    if ((ACCESS_TYPE_WRITE == instr_acc_type) && (INSTR_TYPE_S == instr_type)) {
        if (config.isSqEnabled()) {
            pipeline.push(Instr);
        } else {
            auto *instr_s = dynamic_cast<InstructionS *>(Instr);
            int32_t data = instr_s->get_rs2().get_value();
            auto addr = static_cast<uint64_t>(instr_s->get_store_addr());
            auto size = static_cast<uint64_t>(instr_s->get_store_size());
            memory.write(&data, addr, size);
        }
    }
    if ((ACCESS_TYPE_READ == instr_acc_type) && (INSTR_TYPE_I == instr_type)) {
        auto* instr_i = dynamic_cast<InstructionI*>(Instr);
        int32_t data = 0;
        auto addr = static_cast<uint64_t>(instr_i->get_load_addr());
        auto size = static_cast<uint64_t>(instr_i->get_load_size());
        if (config.isSqEnabled()) {
            data = static_cast<int32_t>(pipeline.searchData(Instr));
        } else {
            memory.read(&data, addr, size);
        }
        InstrName name = instr_i->get_name();
        if (INSTR_NAME_LHU == name
            || INSTR_NAME_LBU == name) {
            data = signExtend(data, size, EXTEND_TYPE_ZERO);
        } else if ( INSTR_NAME_LH == name
                || INSTR_NAME_LB == name
                || INSTR_NAME_LW == name
                ) {
            data = signExtend(data, size, EXTEND_TYPE_SIGN);
        }
    
        Instr->get_rd_ref().set_value(data);
    }
    
    mw->inst = em->instr;
    mw->pc = em->pc;
}

void PerfSim::writeBack() {
    MW* mw = mw_st.getLoadPtr();
    Instruction* Instr = mw->inst;
    if (mw->is_stall || mw->hazard_stall) {
        if (!mw->is_stall  && mw->hazard_stall)
            hu.write(Instr);
        return;
    }
    if (Instr->get_type() == INSTR_TYPE_R
        || Instr->get_type() == INSTR_TYPE_I
        || Instr->get_type()== INSTR_TYPE_U
        || Instr->get_type() == INSTR_TYPE_J
        ) {
        rf.write(Instr->get_rd());
    }
    ic++;
    if (is_dump_trace) {
        dumpInstruction(Instr);
    }
    if (INSTR_NAME_ECALL == Instr->get_name())
        is_stop = true;
}


int32_t PerfSim::signExtend(int32_t num, int32_t size, ExtendType extend_type) {
    if (4 == size)
        return num;
    if (EXTEND_TYPE_ZERO == extend_type) {
        auto mask = static_cast<uint32_t>((0b1 << size) - 1);
        return static_cast<int32_t>(mask & num);
    } else if ( EXTEND_TYPE_SIGN == extend_type) {
        uint32_t sign_bit = static_cast<uint32_t >(num >> (size - 1)) &0b1;
        auto mask = static_cast<uint32_t >((0b1 << size) - 1);
        if ( sign_bit) {
            return static_cast<int32_t>(~mask | num);
        } else {
            return static_cast<int32_t>(mask & num);
        }
    }

    throw std::runtime_error("sign extend error");
}

std::string PerfSim::intToHex( int32_t i ) {
    std::stringstream stream;
    stream << "0x"
           << std::setfill ('0') << std::setw(sizeof(int32_t)*2)
           << std::hex << i;
    return stream.str();
}

void PerfSim::dump() {
    log << std::string("ic : ") << std::to_string(this->ic) << Log::endl;
    log << std::string("clocks : ") << std::to_string(this->clocks) << Log::endl;
    log << std::string("execution time :") << std::to_string(execution_time)<< std::string("ns") << Log::endl;
    log << std::string("KIPS : ") << std::to_string(static_cast<double>(ic * 1000000) / (execution_time)) << Log::endl;
    hu.dump();
}

void PerfSim::dumpInstruction(Instruction *instruction) {
    std::string str_pc(12 - intToHex(instruction->get_pc()).length(), ' ');
    std::string str_name(12 - getInstrName(instruction->get_name()).length(), ' ');
    if (getInstrName(instruction->get_name()) != "NONE") {
        trace << std::to_string(instruction->get_pc()) << Log::endl;
        trace << intToHex(instruction->get_pc()) << str_pc << getInstrName(instruction->get_name()) << str_name;
        if ((instruction->get_type() == INSTR_TYPE_R) || (instruction->get_type() == INSTR_TYPE_I)
            || (instruction->get_type() == INSTR_TYPE_S) || (instruction->get_type() == INSTR_TYPE_B)) {
            std::string str_rs1(12 - getRegName(instruction->get_rs1().get_name()).length(), ' ');
            trace << "rs1 " << getRegName(instruction->get_rs1().get_name()) << str_rs1 << " (" << std::to_string(instruction->get_rs1().get_value()) << ")";
        } else {
            std::string tmp(16, ' ');
            trace << tmp;
        }
        if ((instruction->get_type() == INSTR_TYPE_R) || (instruction->get_type() == INSTR_TYPE_S)
            || (instruction->get_type() == INSTR_TYPE_B)) {
            std::string str_rs2(12 - getRegName(instruction->get_rs2().get_name()).length(), ' ');
            trace << "rs2 " << getRegName(instruction->get_rs2().get_name()) << str_rs2 << " (" << std::to_string(instruction->get_rs2().get_value()) << ")";
        } else {
            std::string tmp(16, ' ');
            trace << tmp;
        }
        if ((instruction->get_type() == INSTR_TYPE_R) || (instruction->get_type() == INSTR_TYPE_I) ||
            (instruction->get_type() == INSTR_TYPE_U) || (instruction->get_type() == INSTR_TYPE_J)) {
            std::string str_rd(12 - getRegName(instruction->get_rd().get_name()).length(), ' ');
            trace << "rd " << getRegName(instruction->get_rd().get_name()) << str_rd << " (" << std::to_string(instruction->get_rd().get_value()) << ")";
        } else {
            std::string tmp(15, ' ');
            trace << tmp;
        }
        if ((instruction->get_type() == INSTR_TYPE_I) || (instruction->get_type() == INSTR_TYPE_S)
            || (instruction->get_type() == INSTR_TYPE_B) || (instruction->get_type() == INSTR_TYPE_U)
            || (instruction->get_type() == INSTR_TYPE_J)) {
            std::string str_imm(12 - intToHex(instruction->get_imm()).length(), ' ');
            trace << "imm " << intToHex(static_cast<int32_t>(instruction->get_imm())) << str_imm;
        } else {
            std::string tmp(16, ' ');
            trace << tmp;
        }
        trace << Log::endl;
    }
}

std::string PerfSim::getRegName(RegName name) {
    std::string name_;
    switch (name) {
        case REGISTER_NAME_ZERO:
            name_ = "zero";
            break;
        case REGISTER_NAME_RA:
            name_ = "ra";
            break;
        case REGISTER_NAME_SP:
            name_ = "sp";
            break;
        case REGISTER_NAME_GP:
            name_ = "gp";
            break;
        case REGISTER_NAME_TP:
            name_ = "tp";
            break;
        case REGISTER_NAME_T0:
            name_ = "t0";
            break;
        case REGISTER_NAME_T1:
            name_ = "t1";
            break;
        case REGISTER_NAME_T2:
            name_ = "t2";
            break;
        case REGISTER_NAME_S0:
            name_ = "s0";
            break;
        case REGISTER_NAME_S1:
            name_ = "s1";
            break;
        case REGISTER_NAME_A0:
            name_ = "a0";
            break;
        case REGISTER_NAME_A1:
            name_ = "a1";
            break;
        case REGISTER_NAME_A2:
            name_ = "a2";
            break;
        case REGISTER_NAME_A3:
            name_ = "a3";
            break;
        case REGISTER_NAME_A4:
            name_ = "a4";
            break;
        case REGISTER_NAME_A5:
            name_ = "a5";
            break;
        case REGISTER_NAME_A6:
            name_ = "a6";
            break;
        case REGISTER_NAME_A7:
            name_ = "a7";
            break;
        case REGISTER_NAME_S2:
            name_ = "s2";
            break;
        case REGISTER_NAME_S3:
            name_ = "s3";
            break;
        case REGISTER_NAME_S4:
            name_ = "s4";
            break;
        case REGISTER_NAME_S5:
            name_ = "s5";
            break;
        case REGISTER_NAME_S6:
            name_ = "s6";
            break;
        case REGISTER_NAME_S7:
            name_ = "s7";
            break;
        case REGISTER_NAME_S8:
            name_ = "s8";
            break;
        case REGISTER_NAME_S9:
            name_ = "s9";
            break;
        case REGISTER_NAME_S10:
            name_ = "s10";
            break;
        case REGISTER_NAME_S11:
            name_ = "s11";
            break;
        case REGISTER_NAME_T3:
            name_ = "t3";
            break;
        case REGISTER_NAME_T4:
            name_ = "t4";
            break;
        case REGISTER_NAME_T5:
            name_ = "t5";
            break;
        case REGISTER_NAME_T6:
            name_ = "t6";
            break;
        case REGISTER_NAME_NONE:
            name_ = "none";
            break;
    }
    return name_;
}

std::string PerfSim::getInstrName(InstrName name) {
    std::string name_;
    switch (name) {
        case INSTR_NAME_LUI:
            name_ = "LUI";
            break;
        case INSTR_NAME_AUIPC:
            name_ = "AUIPC";
            break;
        case INSTR_NAME_JAL:
            name_ = "JAL";
            break;
        case INSTR_NAME_JALR:
            name_ = "JALR";
            break;
        case INSTR_NAME_BEQ:
            name_ = "BEQ";
            break;
        case INSTR_NAME_BNE:
            name_ = "BNE";
            break;
        case INSTR_NAME_BLT:
            name_ = "BLT";
            break;
        case INSTR_NAME_BGE:
            name_ = "BGE";
            break;
        case INSTR_NAME_BLTU:
            name_ = "BLTU";
            break;
        case INSTR_NAME_BGEU:
            name_ = "BGEU";
            break;
        case INSTR_NAME_LB:
            name_ = "LB";
            break;
        case INSTR_NAME_LH:
            name_ = "LH";
            break;
        case INSTR_NAME_LW:
            name_ = "LW";
            break;
        case INSTR_NAME_LBU:
            name_ = "LBU";
            break;
        case INSTR_NAME_LHU:
            name_ = "LHU";
            break;
        case INSTR_NAME_SB:
            name_ = "SB";
            break;
        case INSTR_NAME_SH:
            name_ = "SH";
            break;
        case INSTR_NAME_SW:
            name_ = "SW";
            break;
        case INSTR_NAME_ADDI:
            name_ = "ADDI";
            break;
        case INSTR_NAME_SLTI:
            name_ = "SLTI";
            break;
        case INSTR_NAME_SLTIU:
            name_ = "SLTIU";
            break;
        case INSTR_NAME_XORI:
            name_ = "XORI";
            break;
        case INSTR_NAME_ORI:
            name_ = "ORI";
            break;
        case INSTR_NAME_ANDI:
            name_ = "ANDI";
            break;
        case INSTR_NAME_SLLI:
            name_ = "SLLI";
            break;
        case INSTR_NAME_SRLI:
            name_ = "SRLI";
            break;
        case INSTR_NAME_SRAI:
            name_ = "SRAI";
            break;
        case INSTR_NAME_ADD:
            name_ = "ADD";
            break;
        case INSTR_NAME_SUB:
            name_ = "SUB";
            break;
        case INSTR_NAME_SLL:
            name_ = "SLL";
            break;
        case INSTR_NAME_SLT:
            name_ = "SLT";
            break;
        case INSTR_NAME_SLTU:
            name_ = "SLTU";
            break;
        case INSTR_NAME_XOR:
            name_ = "XOR";
            break;
        case INSTR_NAME_SRL:
            name_ = "SRL";
            break;
        case INSTR_NAME_SRA:
            name_ = "SRA";
            break;
        case INSTR_NAME_OR:
            name_ = "OR";
            break;
        case INSTR_NAME_AND:
            name_ = "AND";
            break;
        case INSTR_NAME_FENCE:
            name_ = "FENCE";
            break;
        case INSTR_NAME_FENCE_I:
            name_ = "FENCE_I";
            break;
        case INSTR_NAME_ECALL:
            name_ = "ECALL";
            break;
        case INSTR_NAME_EBREAK:
            name_ = "EBREAK";
            break;
        case INSTR_NAME_CSRRW:
            name_ = "CSRRW";
            break;
        case INSTR_NAME_CSRRS:
            name_ = "CSRRS";
            break;
        case INSTR_NAME_CSRRC:
            name_ = "CSRRC";
            break;
        case INSTR_NAME_CSRRWI:
            name_ = "CSRRWI";
            break;
        case INSTR_NAME_CSRRSI:
            name_ = "CSRRSI";
            break;
        case INSTR_NAME_CSRRCI:
            name_ = "CSRRCI";
            break;
        case INSTR_NAME_NONE:
            name_ = "NONE";
            break;
    }
    return name_;
}