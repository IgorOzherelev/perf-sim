cmake_minimum_required(VERSION 3.19)
project(perf-sim)

find_library(LIBELF_LIBRARY elf)
if(NOT LIBELF_LIBRARY)
    message(SEND_ERROR "Failed to find libelf")
    return()
endif()


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-maybe-uninitialized")

add_executable(${PROJECT_NAME} src/ElfManager.cpp src/PerfSim.cpp headers/PerfSim.h src/main.cpp headers/ElfManager.h headers/Decoder.h src/Decoder.cpp src/Memory.cpp headers/Memory.h
        src/Log.cpp headers/Log.h headers/Config.h src/Instruction.cpp headers/Instruction.h headers/Stage.h src/Executors.cpp headers/HazardUnit.h src/HazardUnit.cpp src/RegFile.cpp headers/RegFile.h
        src/Pipeline.cpp headers/Pipeline.h headers/types.h headers/Executors.h)

target_link_libraries(${PROJECT_NAME} ${LIBELF_LIBRARY} )
