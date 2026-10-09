#include "skiplist.h"
#include <cstdlib>
#include <ctime>

SkipNode::SkipNode(const std::string& k, const std::string& v, int lvl)
    : key(k), value(v), level(lvl) {
    forward = new SkipNode*[lvl];
    for(int i = 0; i < lvl; i++) {
        forward[i] = nullptr;
    }
}

SkipNode::~SkipNode() {
    delete[] forward;
}

SkipList::SkipList(int maxLvl) : maxLevel(maxLvl), curLevel(0), nodeCount(0) {
    srand((unsigned)time(nullptr));
    header = new SkipNode("", "", maxLevel);
}

SkipList::~SkipList() {
    SkipNode* p = header->forward[0];
    while(p != nullptr) {
        SkipNode* next = p->forward[0];
        delete p;
        p = next;
    }
    delete header;
}

int SkipList::randomLevel() {
    int lvl = 1;
    while(rand() % 2 == 0 && lvl < maxLevel) {
        lvl++;
    }
    return lvl;
}

void SkipList::insert(const std::string& key, const std::string& value) {
    SkipNode* update[8];
    SkipNode* p = header;

    for(int i = curLevel; i >= 0; i--) {
        while(p->forward[i] != nullptr && p->forward[i]->key < key) {
            p = p->forward[i];
        }
        update[i] = p;
    }

    if(p->forward[0] != nullptr && p->forward[0]->key == key) {
        p->forward[0]->value = value;
        return;
    }

    int newLvl = randomLevel();
    if(newLvl > curLevel) {
        for(int i = curLevel + 1; i < newLvl; i++) {
            update[i] = header;
        }
        curLevel = newLvl;
    }

    SkipNode* newNode = new SkipNode(key, value, newLvl);
    for(int i = 0; i < newLvl; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
    nodeCount++;
}

bool SkipList::search(const std::string& key, std::string& val) {
    SkipNode* p = header;
    for(int i = curLevel; i >= 0; i--) {
        while(p->forward[i] != nullptr && p->forward[i]->key < key) {
            p = p->forward[i];
        }
    }
    p = p->forward[0];
    if(p != nullptr && p->key == key) {
        val = p->value;
        return true;
    }
    return false;
}

void SkipList::remove(const std::string& key) {
    SkipNode* update[8];
    SkipNode* p = header;

    for(int i = curLevel; i >= 0; i--) {
        while(p->forward[i] != nullptr && p->forward[i]->key < key) {
            p = p->forward[i];
        }
        update[i] = p;
    }

    p = p->forward[0];
    if(p == nullptr || p->key != key) return;

    for(int i = 0; i <= curLevel; i++) {
        if(update[i]->forward[i] != p) break;
        update[i]->forward[i] = p->forward[i];
    }
    delete p;
    nodeCount--;

    while(curLevel > 0 && header->forward[curLevel] == nullptr) {
        curLevel--;
    }
}

int SkipList::getSize() {
    return nodeCount;
}

void SkipList::clear() {
    SkipNode* p = header->forward[0];
    while(p != nullptr) {
        SkipNode* next = p->forward[0];
        delete p;
        p = next;
    }
    for(int i = 0; i < maxLevel; i++) {
        header->forward[i] = nullptr;
    }
    curLevel = 0;
    nodeCount = 0;
}
