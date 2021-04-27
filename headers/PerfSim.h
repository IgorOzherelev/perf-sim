#ifndef __H
#define __H

#include<string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "ElfManager.h"
#include "Memory.h"
#include "Instruction.h"
#include "Config.h"
#include "Decoder.h"
#include "Stage.h"
#include "HazardUnit.h"
#include "RegFile.h"
#include "Pipeline.h"

enum ExtendType {
    EXTEND_TYPE_SIGN,
    EXTEND_TYPE_ZERO
};

class PerfSim {
private:
    Config config;
    Log::Logger& log;
    Log::Logger& trace;
    Memory memory;
    RegFile rf;
    Stage<WF> fetch_st;
    Stage<FD> fd_st;
    Stage<DE> de_st;
    Stage<EM> em_st;
    Stage<MW> mw_st;
    HazardUnit hu;
    Decoder decoder;
    Pipeline pipeline;
    uint64_t ic = 0;
    uint64_t clocks = 0;
    bool is_stop = false;
    bool is_dump_trace = false;
    long int execution_time = 0;
    static int32_t signExtend(int32_t num, int32_t size, ExtendType extend_type);
    void dumpInstruction(Instruction* instruction);
    static std::string getInstrName(InstrName name);
    static std::string getRegName(RegName name);
    static std::string intToHex(int32_t i);
    void dump();
    void fetch();
    void decode();
    void execute();
    void memoryAccess();
    void writeBack();
public:
    void run();
    explicit PerfSim( const std::string&  path_to_test);
    ~PerfSim() = default;
};

#endif
