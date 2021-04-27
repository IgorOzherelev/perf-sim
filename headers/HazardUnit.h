#ifndef _HAZARDUNIT_H
#define _HAZARDUNIT_H

#include "Config.h"
#include "Stage.h"
#include "Instruction.h"
#include "RegFile.h"


class HazardUnit {
    uint32_t bypasses_mem_ex = 0;
    uint32_t bypasses_wb_ex = 0;
    uint32_t branches = 0;
    uint32_t stops = 0;
    Config& config;

    RegFile& rf;
    Stage<FD>& fd_st;
    Stage<DE>& de_st;
    Stage<EM>& em_st;
    Stage<MW>& mw_st;
    Stage<WF>& wf_st;

    static inline bool is_load(Instruction* instruction);
   
public:
    void write(Instruction* instruction);
    HazardUnit(Config&, RegFile&, Stage <FD>&,  Stage <DE>&, Stage <EM>&, Stage <MW>&, Stage <WF>&);
    Register decode(Register rs, PipelineStage need_value_on);
    void branch(Instruction* instruction);
    void exit(Instruction* ecall);
    void dump();
    ~HazardUnit() = default;
};
#endif
