#ifndef WAL_H
#define WAL_H

#include <string>
#include "skiplist.h"

enum class OpType {
    SET,
    DEL
};

class WAL {
private:
    std::string filename;
    FILE* fp;
    bool enable;

public:
    WAL(const std::string& file);
    ~WAL();

    void setEnable(bool on);
    bool isEnable() const;

    bool writeLog(OpType op, const std::string& key, const std::string& val = "");
    void flush();

    bool recover(SkipList* sk);
    void clear();
};

#endif
