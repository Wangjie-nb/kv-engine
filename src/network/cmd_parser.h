#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <string>
#include "core/kvstore.h"

class CmdParser {
private:
    KVStore* kv;

public:
    CmdParser(KVStore* k);

    std::string handleLine(const std::string& line);
};

#endif
