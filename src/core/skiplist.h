#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <string>

struct SkipNode {
    std::string key;
    std::string value;
    SkipNode** forward;
    int level;
    SkipNode(const std::string& k, const std::string& v, int lvl);
    ~SkipNode();
};

class SkipList {
private:
    SkipNode* header;
    int maxLevel;
    int curLevel;
    int nodeCount;
    int randomLevel();

public:
    SkipList(int maxLvl = 8);
    ~SkipList();

    void insert(const std::string& key, const std::string& value);
    bool search(const std::string& key, std::string& val);
    void remove(const std::string& key);

    int getSize();
    void clear();
};

#endif
