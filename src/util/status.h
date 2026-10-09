#ifndef STATUS_H
#define STATUS_H

#include <string>

class Status {
public:
    enum Code {
        OK = 0,
        NOT_FOUND = 1,
        INVALID_ARG = 2,
        IO_ERROR = 3,
        CORRUPTION = 4,
    };

    Status() : code_(OK) {}
    Status(Code c) : code_(c) {}

    bool ok() const { return code_ == OK; }
    Code code() const { return code_; }

    std::string toString() const {
        switch(code_) {
            case OK: return "OK";
            case NOT_FOUND: return "Not found";
            case INVALID_ARG: return "Invalid argument";
            case IO_ERROR: return "IO error";
            case CORRUPTION: return "Corruption";
            default: return "Unknown";
        }
    }

private:
    Code code_;
};

#endif
