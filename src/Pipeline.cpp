#include "../headers/Pipeline.h"


Pipeline::Pipeline(Memory& memory, Config& config): memory(memory), config(config) {}

void Pipeline::push(Instruction* instruction) {
    if (instruction->get_mem_acc_type() != ACCESS_TYPE_WRITE)
        throw std::runtime_error("Wrong access type");
    auto* instr_s = dynamic_cast<InstructionS*>(instruction);
    store.data = static_cast<uint32_t>(instr_s->get_rs2().get_value());
    store.addr = static_cast<uint64_t>(instr_s->get_store_addr());
    store.size = static_cast<uint64_t>(instr_s->get_store_size());
    id++;
    store.id = id;
    if (tryConcatenate(store)) {
        store.clear();
    }
}

uint32_t Pipeline::searchData(Instruction* instruction) {
    if (instruction->get_mem_acc_type() != ACCESS_TYPE_READ)
        throw std::runtime_error("Wrong access type");
    auto* instr_i = dynamic_cast<InstructionI*>(instruction);
    
    uint32_t id_ = queue.begin()->id;
    Store ld;
    ld.data = 0;
    ld.addr = static_cast<uint64_t>(instr_i->get_load_addr());
    ld.size = static_cast<uint64_t>(instr_i->get_load_size());
    std::vector<bool> fullness(32,false);
    
    for (auto it = queue.begin(); it != queue.end(); it++) {
        if (it->id < id_)
            throw 1;
        id_ = it->id;
       
        
        if ((it->addr + it->size) >= ld.addr &&
            (it->addr + it->size) <= (ld.addr + ld.size)) {
            if (ld.addr >= it->addr) {
                std::memcpy( (&ld.data), (char*)(&(it->data)) + ld.addr - it->addr,it->addr + it->size - ld.addr);
                for (uint32_t index = 0; index < it->addr + it->size - ld.addr; index++) {
                    fullness[index] = true;
                }
            }
            else {
                std::memcpy( (char*)(&ld.data) + (it->addr - ld.addr), &(it->data), it->size);
    
                for (uint64_t index =  (it->addr - ld.addr); index <  (it->addr - ld.addr) + it->size; index++) {
                    fullness[index] = true;
                }
            }
            continue;
        }
    
        if ((it->addr) >= ld.addr &&
            (it->addr) <= (ld.addr + ld.size)) {
            if (ld.addr + ld.size <= it->addr + it->size) {
                std::memcpy((char*)(&ld.data) + (it->addr - ld.addr), &(it->data),ld.addr + ld.size - it->addr );
                for (uint64_t index =  (it->addr - ld.addr); index <  (it->addr - ld.addr) + ld.addr + ld.size - it->addr ; index++) {
                    fullness[index] = true;
                }
            }
            else {
                std::memcpy( (char*)(&ld.data) + (it->addr - ld.addr), &(it->data),it->size);
                for (uint64_t index =  (it->addr - ld.addr); index <  (it->addr - ld.addr) + it->size ; index++) {
                    fullness[index] = true;
                }
            }
            continue;
        
        }
        if (ld.addr >= it->addr  &&
            ld.addr+ld.size  <= it->addr + it->size) {
            std::memcpy(&ld.data,(char*)(&it->data) + (ld.addr - it->addr), ld.size);
            for(unsigned idx = 0; idx < fullness.size(); idx++) {
                fullness[idx] = true;
            }
            continue;
        }
        
    }
    uint32_t mask  = 0;
    for ( uint32_t i = 0 ; i < fullness.size(); i++) {
        if (!fullness[i]) {
            mask += 1 <<i;
        }
    }
    uint32_t d_data;
    memory.read(&d_data, ld.addr, ld.size);
    ld.data+= mask&d_data;
    return ld.data;
}

bool Pipeline::tryConcatenate(Store st) {
    bool res = false;
    uint32_t id_ = queue.rbegin()->id;
    for (auto it = queue.rbegin(); it != queue.rend(); it++) {
        if (it->id > id_)
            throw std::runtime_error("Pipeline wrong id");
        id_ = it->id;

        if ((it->addr + it->size) >= st.addr &&
            (it->addr + it->size) <= (st.addr + st.size))
        {
            uint64_t addr = std::min(it->addr, st.addr);
            uint64_t size_ = st.addr + st.size - addr;
            if (size_ > 32)
                return res;
            st.addr = addr;
            st.size = size_;
            st.id = it->id;
            if (st.addr >= it->addr) {
                st.data = (st.data>>(st.addr - it->addr) );
                std::memcpy(&st.data, &(it->data), st.addr - it->addr);
                *it = st;
            } else {
                uint32_t data = st.data;
                st.data = data;
                *it = st;
            }
            res = true;
            continue;
        }

        if ((it->addr) >= st.addr &&
            (it->addr) <= (st.addr + st.size)) {
            uint64_t addr = st.addr;
            uint64_t size_ = std::max(it->addr + it->size, st.addr + st.size) - addr;
            if (size_ > 32)
                return res;
            st.addr = addr;
            st.size = size_;
            st.id = it->id;
            if (st.addr + st.size <= it->addr + it->size) {
                uint32_t data = it->data >> (it->addr - st.addr);
                std::memcpy(&data, &(st.data), st.size);
                st.data = data;
                *it = st;
            } else {
                uint32_t data = st.data;
                st.data = data;
                *it = st;
            }
            res = true;
            continue;

        }
        if (st.addr >= it->addr  &&
            st.addr+st.size  <= it->addr + it->size) {
            uint64_t addr = it->addr;
            uint64_t size_ = it->size;
            uint32_t  data = it->data;
            st.addr = addr;
            st.size = size_;
            st.id = it->id;
            std::memcpy((char*)(&data) + ( st.addr - it->addr ), &(st.data) , st.size);
            st.data = data;
            *it = st;
            res = true;
            continue;
        }
    }
    return res;
}

void Pipeline::update() {
    queue.push_back(store);
    Store store_tmp = queue.front();
    if (store_tmp.size > 0) {
        memory.write(&store_tmp.data, store_tmp.addr, store_tmp.size);
    }
    queue.pop_front();
    store.clear();
}