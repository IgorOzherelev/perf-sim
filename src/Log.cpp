#include "../headers/Log.h"

using namespace Log;

Log::Logger::Logger(): path(def_path) {
    stream = new std::ofstream();
}

void Logger::check_is_null() {
    if (stream == nullptr)
        throw std::runtime_error("Log check is null error");
}

void Logger::open(std::string path_) {
    check_is_null();
    if (this->path != path_ )
        this->path = path_;
    if (stream->is_open())
        stream->close();
    stream->open(path_);
    if (!stream->is_open())
        throw std::runtime_error("Could not open Log");
}

void Logger::close() {
    check_is_null();
    if (stream->is_open())
        stream->close();
}

void Logger::err(std::string &message) {
    check_is_null();
    *stream << "error: " << message;
}

void Logger::info(std::string &message) {
    check_is_null();
    *stream << message;
}

void Logger::warn(std::string &message) {
    check_is_null();
    *stream << "warning: " <<message;
}

void Logger::endl() {
    check_is_null();
    *stream << std::endl;
}
Logger& Logger::operator<<(const std::string &mess1) {
    check_is_null();
    *stream << mess1;
    return *this;
}

Logger& Logger::operator<<(const Log::LogMod &mod) {
    switch (mod.get_mode()) {
        case INFO:
            info(mess);
            break;
        case WARN:
            warn(mess);
            break;
        case ERR:
            err(mess);
            break;
        case ENDL:
            endl();
            break;
        default:
            throw std::runtime_error("Log error");
    }
    return *this;
}

Log::Logger::~Logger() {
    this->close();
    check_is_null();
    delete stream;
}
