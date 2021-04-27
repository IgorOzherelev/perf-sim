#include "../headers/Decoder.h"

Decoder::Decoder(Config& config, HazardUnit& hz) : config(config), hu(hz) , pipeline_size(config.getNumOfPipelineStages()),
                                                   i_instr_arr(pipeline_size), b_instr_arr(pipeline_size), r_instr_arr(pipeline_size), s_instr_arr(pipeline_size),
                                                   u_instr_arr(pipeline_size), j_instr_arr(pipeline_size), sys_instr_arr(pipeline_size){};

Instruction* Decoder::decode(uint32_t instr_, RegFile& reg) {
    this->instr = instr_;
    Instruction* instruction;
    InstructionI* instr_i;
    InstructionB* instr_b;
    InstructionR* instr_r;
    InstructionS* instr_s;
    InstructionU* instr_u;
    InstructionJ* instr_j;
    name = INSTR_NAME_NONE;
    type=  INSTR_TYPE_NONE;
    mem_acc_type = ACCESS_TYPE_NONE;
    executor = nullptr;
    acc_size = 0;
    is_branch = false;

    uint32_t funct3 = (instr_ >> 12) & 0b0111u;
    uint32_t funct7 = (instr_ >> 25) & 0b01111111u;
    uint32_t opcode = instr_ & 0b1111111u;
    auto num_rs1 = static_cast<RegName>((instr_ >> 15) & 0b011111u);
    auto num_rs2 = static_cast<RegName>((instr_ >> 20) & 0b011111u);
    auto num_rd = static_cast<RegName>((instr_ >> 7) & 0b011111u);

    recognize(opcode, funct3, funct7);
    switch (type) {
    case INSTR_TYPE_R:
        instr_r = r_instr_arr.get_next();
        if (reg.is_lock(num_rs1)) {
            instr_r->rs1  = hu.decode(reg.getReg(num_rs1), PIPELINE_STAGE_EXECUTE);
        } else {
            instr_r->rs1 = reg.getReg(num_rs1);
        }
        
        if (reg.is_lock(num_rs2)) {
            instr_r->rs2 = hu.decode(reg.getReg(num_rs2), PIPELINE_STAGE_EXECUTE);
        } else {
            instr_r->rs2 = reg.getReg(num_rs2);
        }
        instr_r->rd = reg.getReg(num_rd, ACCESS_TYPE_WRITE);
        instruction = dynamic_cast<Instruction*>(instr_r);
        break;
    case INSTR_TYPE_I:
        instr_i = i_instr_arr.get_next();
        instr_i->load_size = acc_size;
        instruction = dynamic_cast<Instruction*>(instr_i);
        
        if (reg.is_lock(num_rs1)) {
            instr_i->rs1 = hu.decode(reg.getReg(num_rs1), PIPELINE_STAGE_EXECUTE);
        } else {
            instr_i->rs1 = reg.getReg(num_rs1);
        }
        instr_i->rd = reg.getReg(num_rd, ACCESS_TYPE_WRITE);
        break;
    case INSTR_TYPE_S:
        instr_s = s_instr_arr.get_next();
        instr_s->store_size = acc_size;
        instruction = dynamic_cast<Instruction*>(instr_s);
        if (reg.is_lock(num_rs1)) {
            instr_s->rs1 = hu.decode(reg.getReg(num_rs1), PIPELINE_STAGE_EXECUTE);
        } else {
            instr_s->rs1 = reg.getReg(num_rs1);
        }
        if (reg.is_lock(num_rs2)) {
            if (ACCESS_TYPE_WRITE == mem_acc_type) {
                instr_s->rs2 = hu.decode(reg.getReg(num_rs2), PIPELINE_STAGE_MEMORY);
            } else {
                instr_s->rs2 = hu.decode(reg.getReg(num_rs2), PIPELINE_STAGE_EXECUTE);
            }
        } else {
            instr_s->rs2 = reg.getReg(num_rs2);
        }
        break;
    case INSTR_TYPE_B:
        instr_b = b_instr_arr.get_next();
        instruction = dynamic_cast<Instruction*>(instr_b);
        if (reg.is_lock(num_rs1)) {
            instr_b->rs1 = hu.decode(reg.getReg(num_rs1), PIPELINE_STAGE_EXECUTE);
        } else {
            instr_b->rs1 = reg.getReg(num_rs1);
        }

        if (reg.is_lock(num_rs2)) {
            instr_b->rs2 = hu.decode(reg.getReg(num_rs2), PIPELINE_STAGE_EXECUTE);
        } else {
            instr_b->rs2 = reg.getReg(num_rs2);
        }
        break;
    case INSTR_TYPE_U:
        instr_u = u_instr_arr.get_next();
        instruction = dynamic_cast<Instruction*>(instr_u);
        instr_u->rd = reg.getReg(num_rd, ACCESS_TYPE_WRITE);
        break;
    case INSTR_TYPE_J:
        instr_j = j_instr_arr.get_next();
        instruction = dynamic_cast<Instruction*>(instr_j);
        instr_j->rd = reg.getReg(num_rd, ACCESS_TYPE_WRITE);
        break;
    case INSTR_TYPE_SYSTEM:
        instruction = sys_instr_arr.get_next();
        break;
    default:
        instruction = new Instruction;
    }

    instruction->name = this->name;
    instruction->type = this->type;
    instruction->main_executor = executor;
    instruction->opcode = opcode;
    instruction->is_branch = is_branch;
    instruction->mem_acc_type = mem_acc_type;
    instruction->branch_addr = 0;
    instruction->calc_imm(instr_);

    return instruction;
}
void Decoder::recognize(uint32_t opcode, uint32_t funct3, uint32_t funct7) {
    switch (opcode) {
    case 0b0110111://LUI
        name = INSTR_NAME_LUI;
        type = INSTR_TYPE_U;
        executor = &(Executors::LUI);
        break;
    case 0b0010111://AUIPC
        name = INSTR_NAME_AUIPC;
        type = INSTR_TYPE_U;
        executor = &(Executors::AUIPC);
        break;
    case 0b1101111:
        is_branch =true;
        name = INSTR_NAME_JAL;
        type = INSTR_TYPE_J;
        executor = &(Executors::JAL);
        break;
    case 0b1100111:
        is_branch = true;
        name = INSTR_NAME_JALR;
        executor = &(Executors::JALR);
        type = INSTR_TYPE_I;
        break;
    case 0b1100011:
        type = INSTR_TYPE_B;
        is_branch = true;
        switch (funct3) {
        case 0b000:
            name = INSTR_NAME_BEQ;
            executor = &(Executors::BEQ);
            break;
        case 0b001:
            name = INSTR_NAME_BNE;
            executor = &(Executors::BNE);
            break;
        case 0b100:
            name = INSTR_NAME_BLT;
            executor = &(Executors::BLT);
            break;
        case 0b101:
            name = INSTR_NAME_BGE;
            executor = &(Executors::BGE);
            break;
        case 0b110:
            name = INSTR_NAME_BLTU;
            executor = &(Executors::BLTU);
            break;
        case 0b111:
            name = INSTR_NAME_BGEU;
            executor = &(Executors::BGEU);
            break;
        }
        break;
    case 0b0000011:
        type = INSTR_TYPE_I;
        executor = &(Executors::Load);
        switch (funct3) {
        case 0b000:
            name = INSTR_NAME_LB;
            mem_acc_type = ACCESS_TYPE_READ;
            acc_size =1;
            break;
        case 0b001:
            name = INSTR_NAME_LH;
            mem_acc_type = ACCESS_TYPE_READ;
            acc_size =2;
            break;
        case 0b010:
            name = INSTR_NAME_LW;
            mem_acc_type = ACCESS_TYPE_READ;
            acc_size =4;
            break;
        case 0b100:
            name = INSTR_NAME_LBU;
            mem_acc_type = ACCESS_TYPE_READ;
            acc_size = 1;
            break;
        case 0b101:
            name = INSTR_NAME_LHU;
            mem_acc_type = ACCESS_TYPE_READ;
            acc_size = 2;
            break;
        }
        break;
    case 0b0100011:
        type = INSTR_TYPE_S;
        switch (funct3) {
        case 0b000:
            name = INSTR_NAME_SB;
            mem_acc_type = ACCESS_TYPE_WRITE;
            acc_size = 1;
            executor = &(Executors::Store);
            break;
        case 0b001:
            name = INSTR_NAME_SH;
            mem_acc_type = ACCESS_TYPE_WRITE;
            acc_size = 2;
            executor = &(Executors::Store);
            break;
        case 0b010:
            name = INSTR_NAME_SW;
            mem_acc_type = ACCESS_TYPE_WRITE;
            acc_size = 4;
            executor = &(Executors::Store);
            break;
        }
        break;
    case  0b0010011:
        switch (funct3) {
        case 0b001: //SLLI
            type = INSTR_TYPE_I;
            name = INSTR_NAME_SLLI;
            executor = &(Executors::SLLI);
            break;
        case 0b101:
            switch (funct7) {
                case 0b0000000:
                    type = INSTR_TYPE_I;
                    name = INSTR_NAME_SRLI;
                    executor = &(Executors::SRLI);
                    break;
                case 0b0100000:
                    type = INSTR_TYPE_I;
                    name = INSTR_NAME_SRAI;
                    executor = &(Executors::SRAI);
                    break;
            }
            break;
        case 0b000:
            type = INSTR_TYPE_I;
            name = INSTR_NAME_ADDI;
            executor = &(Executors::ADDI);
            break;
        case 0b010:
            type = INSTR_TYPE_I;
            name = INSTR_NAME_SLTI;
            executor = &(Executors::SLTI);
            break;
        case 0b011:
            type = INSTR_TYPE_I;
            name = INSTR_NAME_SLTIU;
            executor = &(Executors::SLTIU);
            break;
        case 0b100:
            type = INSTR_TYPE_I;
            name = INSTR_NAME_XORI;
            executor = &(Executors::XORI);
            break;
        case 0b110:
            type = INSTR_TYPE_I;
            name = INSTR_NAME_ORI;
            executor = &(Executors::ORI);
            break;
        case 0b111:
            type = INSTR_TYPE_I;
            name = INSTR_NAME_ANDI;
            executor = &(Executors::ANDI);
            break;
        }
        break;
    case 0b0110011:
        type = INSTR_TYPE_R;
        switch (funct3) {
        case 0b000:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_ADD;
                executor = &(Executors::ADD);
                break;
            case 0b0100000:
                name = INSTR_NAME_SUB;
                executor = &(Executors::SUB);
                break;
            }
            break;
        case 0b001:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_SLL;
                executor = &(Executors::SLL);
                break;
            }
            break;
        case 0b010:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_SLT;
                executor = &(Executors::SLT);
                break;
            }
            break;
        case 0b011:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_SLTU;
                executor = &(Executors::SLTU);
                break;
            }
            break;
        case 0b100:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_XOR;
                executor = &(Executors::XOR);
                break;
            }
            break;
        case 0b101:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_SRL;
                executor = &(Executors::SRL);
                break;
            case 0b0100000:
                name = INSTR_NAME_SRA;
                    executor = &(Executors::SRA);
                break;
            }
            break;
        case 0b110:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_OR;
                executor = &(Executors::OR);
                break;
            }
            break;
        case 0b111:
            switch (funct7) {
            case 0b0000000:
                name = INSTR_NAME_AND;
                executor = &(Executors::AND);
                break;
            }
            break;
        }
        break;
    case 0b1110011:
        type = INSTR_TYPE_SYSTEM;
        if (instr == 0b1110011) {
    
            name = INSTR_NAME_ECALL;
            executor = &(Executors::ECALL);
        }
        break;
    }
}
