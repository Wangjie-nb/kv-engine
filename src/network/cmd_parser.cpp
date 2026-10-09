#include "cmd_parser.h"

CmdParser::CmdParser(KVStore* k) : kv(k) {}

std::string CmdParser::handleLine(const std::string& line) {
    size_t spacePos = line.find(' ');
    if(spacePos == std::string::npos) {
        if(line == "size") {
            return std::to_string(kv->size()) + "\n> ";
        } else if(line == "clear") {
            kv->clear();
            return "ok\n> ";
        } else {
            return "error: unknown command\n> ";
        }
    }

    std::string op = line.substr(0, spacePos);
    size_t secondSpace = line.find(' ', spacePos + 1);

    if(op == "get") {
        std::string key = line.substr(spacePos + 1);
        std::string val;
        if(kv->get(key, val)) {
            return val + "\n> ";
        } else {
            return "not found\n> ";
        }
    } else if(op == "set") {
        if(secondSpace == std::string::npos) {
            return "error: usage: set key value\n> ";
        }
        std::string key = line.substr(spacePos + 1, secondSpace - spacePos - 1);
        std::string val = line.substr(secondSpace + 1);
        kv->set(key, val);
        return "ok\n> ";
    } else if(op == "del") {
        std::string key = line.substr(spacePos + 1);
        kv->del(key);
        return "ok\n> ";
    } else if(op == "exists") {
        std::string key = line.substr(spacePos + 1);
        if(kv->exists(key)) {
            return "yes\n> ";
        } else {
            return "no\n> ";
        }
    } else {
        return "error: unknown command\n> ";
    }
}
