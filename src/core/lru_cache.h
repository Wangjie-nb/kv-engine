#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <string>

struct CacheNode {
    std::string key;
    std::string value;
    CacheNode* prev;
    CacheNode* next;
    CacheNode(const std::string& k = "", const std::string& v = "")
        : key(k), value(v), prev(nullptr), next(nullptr) {}
};

class LRUCache {
private:
    int capacity;
    int count;
    std::unordered_map<std::string, CacheNode*> hashMap;
    CacheNode* head;
    CacheNode* tail;

    void removeNode(CacheNode* node);
    void addToHead(CacheNode* node);
    void moveToHead(CacheNode* node);
    CacheNode* removeTail();

public:
    LRUCache(int cap);
    ~LRUCache();

    bool get(const std::string& key, std::string& val);
    void put(const std::string& key, const std::string& val);
    bool del(const std::string& key);
    void clear();
};

#endif
