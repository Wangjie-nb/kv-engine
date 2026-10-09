#include "wal.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

WAL::WAL(const std::string& file) : filename(file), fp(nullptr), enable(true) {
    fp = fopen(filename.c_str(), "a+");
}

WAL::~WAL() {
    if(fp != nullptr) {
        fflush(fp);
        fclose(fp);
    }
}

void WAL::setEnable(bool on) {
    enable = on;
}

bool WAL::isEnable() const {
    return enable;
}

bool WAL::writeLog(OpType op, const std::string& key, const std::string& val) {
    if(!enable || fp == nullptr) return false;

    bool ok = false;
    if(op == OpType::SET) {
        ok = (fprintf(fp, "SET %s %s\n", key.c_str(), val.c_str()) >= 0);
    } else if(op == OpType::DEL) {
        ok = (fprintf(fp, "DEL %s\n", key.c_str()) >= 0);
    }
    return ok;
}

void WAL::flush() {
    if(fp != nullptr) {
        fflush(fp);
        fsync(fileno(fp));
    }
}

bool WAL::recover(SkipList* sk) {
    if(sk == nullptr || fp == nullptr) return false;

    fseek(fp, 0, SEEK_SET);

    char line[1024];
    while(fgets(line, sizeof(line), fp) != nullptr) {
        size_t len = strlen(line);
        while(len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }
        if(len == 0) continue;

        if(strncmp(line, "SET ", 4) == 0) {
            char* p = line + 4;
            char* sp = strchr(p, ' ');
            if(sp == nullptr) continue;
            *sp = '\0';
            sk->insert(p, sp + 1);
        } else if(strncmp(line, "DEL ", 4) == 0) {
            sk->remove(line + 4);
        }
    }

    fseek(fp, 0, SEEK_END);
    return true;
}

void WAL::clear() {
    if(fp != nullptr) fclose(fp);
    fp = fopen(filename.c_str(), "w");
}
