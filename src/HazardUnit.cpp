#include "../headers/HazardUnit.h"


HazardUnit::HazardUnit(Config& config, RegFile& rf, Stage <FD>& fd_cell, Stage <DE>& de_cell, Stage <EM>& em_cell, Stage <MW>& mw_cell, Stage <WF>& wf_cell):
        config(config), rf(rf), fd_st(fd_cell), de_st(de_cell), em_st(em_cell), mw_st(mw_cell), wf_st(wf_cell) {
    (void) this->config.getPageSize();
    (void)this->mw_st.store;
    (void)this->fd_st.store;
    (void)this->de_st.store;
}

Register HazardUnit::decode(Register rs, PipelineStage need_value_on) {
    DE *de = de_st.getLoadPtr();
    bool stall = de->is_stall || de->hazard_stall;
    Instruction* instr = de->instr;
    if (!stall &&
            (instr->get_type() == INSTR_TYPE_R
            || instr->get_type() == INSTR_TYPE_I
            || instr->get_type() == INSTR_TYPE_U
            || instr->get_type() == INSTR_TYPE_J)
        ) {
        Register rd = instr->get_rd();
        if (rd.get_name() == rs.get_name()) {
            if (is_load(instr)) {
                if (PIPELINE_STAGE_EXECUTE == need_value_on) {
                    stops++;
                    bypasses_wb_ex++;
                    fd_st.extend(1);
                    wf_st.extend(1);
                    em_st.store->hazard_stall =true;
                    return rs;
                }
                throw std::runtime_error("load hazart");
            }
            bypasses_mem_ex++;
            return rd;
        }
    }
    
    EM *em = em_st.getLoadPtr();
    instr = em->instr;
    stall = em->is_stall || em->hazard_stall;
    if (!stall &&
        (instr->get_type() == INSTR_TYPE_R
            || instr->get_type() == INSTR_TYPE_I
            || instr->get_type() == INSTR_TYPE_U
            || instr->get_type() == INSTR_TYPE_J)
        ) {
        Register rd = instr->get_rd();
        if (rd.get_name() == rs.get_name()) {
            bypasses_wb_ex++;
            return rd;
        }
    }

    return Register();
}

void HazardUnit::branch(Instruction *instruction) {
   WF* wf = wf_st.getStorePtr();
   wf->is_jump = true;
   wf->pc_jump = static_cast<int32_t >(instruction->get_br_target_addr());
   FD* fd = fd_st.getStorePtr();
   fd->hazard_stall = true;
   DE* de = de_st.getStorePtr();
   de->hazard_stall = true;
    
    branches++;
}

inline bool HazardUnit::is_load(Instruction* instruction) {
    return (instruction->get_mem_acc_type() == ACCESS_TYPE_READ);
}

void HazardUnit::write(Instruction *instruction) {
    InstrType type = instruction->get_type();
    if (INSTR_TYPE_R == type
        || INSTR_TYPE_I == type
        || INSTR_TYPE_U == type
         || INSTR_TYPE_J == type) {
        rf.write(instruction->get_rd());
    }
}
void HazardUnit::exit(Instruction *ecall) {
    (void)ecall;
    wf_st.extend(5);
    fd_st.extend(5);
    de_st.extend(5);
    de_st.load->hazard_stall= true;
}

void HazardUnit::dump() {
    Log::Logger& log = config.getLogRef();
    log<<std::string("bypasses_wb_ex: ")<<std::to_string(this->bypasses_wb_ex) << Log::endl;
    log<<std::string("bypasses_mem_ex: ")<<std::to_string(this->bypasses_mem_ex) << Log::endl;
    log<<std::string("pipeline stops: ")<<std::to_string(this->stops) << Log::endl;
    log<<std::string("branches: ")<<std::to_string(this->branches) << Log::endl;
}
