#ifndef _LOG_H
#define _LOG_H

#include <string>
#include <iostream>
#include <fstream>

namespace Log {
    enum PrintType {
        ERR,
        INFO,
        WARN,
        ENDL
    };
    class LogMod {
        int mod;
    public:

        PrintType get_mode() const {return static_cast<PrintType>(mod);};
        explicit LogMod(int mod): mod(mod) {};
        ~LogMod() = default;
    };
    const LogMod endl(PrintType::ENDL);
    const LogMod err(PrintType::ERR);
    const LogMod info(PrintType::INFO);
    const LogMod warn(PrintType::WARN);
    const std::string def_path("Log.txt");
    class Logger {
        std::string path ;
        std::string mess;
        std::ofstream* stream = nullptr;
        void check_is_null();
    public:
        void open(std::string path_);
        void err(std::string& message);
        void warn(std::string& message);
        void info(std::string& message);
        void endl();
        void close();
        Logger& operator << (const std::string& mess);
        Logger& operator << (const LogMod& mod);

        Logger();
        ~Logger();
    };
}
#endif

