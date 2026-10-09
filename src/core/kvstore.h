#ifndef KVSTORE_H
#define KVSTORE_H

#include <string>
#include <mutex>
#include "skiplist.h"
#include "lru_cache.h"
#include "wal.h"

class KVStore {
private:
    SkipList* skipList;
    LRUCache* lruCache;
    WAL* wal;
    std::mutex mtx;

public:
    KVStore(int lruCap);
    ~KVStore();

    void set(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& val);
    void del(const std::string& key);
    bool exists(const std::string& key);

    void bindWAL(WAL* w);
    SkipList* getSkipList();

    int size();
    void clear();
};

#endif
