#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include "Log.h"


class Config {
private:
    uint64_t page_size = 0;
    uint64_t memory_size = page_size * 1024;
    uint32_t stages = 0;
    uint64_t num_regs = 0;
    bool is_log = true;
    Log::Logger log;
    Log::Logger trace;
    bool dump_trace = false;
    bool is_sq = false;

public:
    uint64_t getMemSize() const{ return memory_size;};
    uint64_t getPageSize() const{ return page_size;};
    uint64_t getNumRegs() const{return num_regs;};
    uint32_t getNumOfPipelineStages() const{return stages;}
    bool getLog() const{return is_log;};
    bool isDumpTrace() const {return dump_trace;}
    bool isSqEnabled() const {return is_sq;}
    Log::Logger& getLogRef() {return log;};
    Log::Logger& getTraceRef() {return trace;}
    explicit Config() {
        this->is_log = true;
        this->dump_trace = true;
        this->is_sq = true;
        this->page_size = 4096;
        this->memory_size = 4096 * 1024;
        this->num_regs = 32;
        this->stages = 5;

        if (dump_trace)
            trace.open("trace.txt");
        if (is_log)
            log.open("Log.txt");
    };
    ~Config() = default;
};

#endif //CONFIG_H
