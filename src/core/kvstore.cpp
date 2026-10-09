#include "kvstore.h"

KVStore::KVStore(int lruCap) {
    skipList = new SkipList();
    lruCache = new LRUCache(lruCap);
    wal = nullptr;
}

KVStore::~KVStore() {
    delete skipList;
    delete lruCache;
}

void KVStore::bindWAL(WAL* w) {
    wal = w;
}

SkipList* KVStore::getSkipList() {
    return skipList;
}

void KVStore::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);

    if(wal != nullptr && wal->isEnable()) {
        wal->writeLog(OpType::SET, key, value);
        wal->flush();
    }
    skipList->insert(key, value);
    lruCache->put(key, value);
}

bool KVStore::get(const std::string& key, std::string& val) {
    std::lock_guard<std::mutex> lock(mtx);

    if(lruCache->get(key, val)) {
        return true;
    }
    if(skipList->search(key, val)) {
        lruCache->put(key, val);
        return true;
    }
    return false;
}

void KVStore::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);

    if(wal != nullptr && wal->isEnable()) {
        wal->writeLog(OpType::DEL, key);
        wal->flush();
    }
    skipList->remove(key);
    lruCache->del(key);
}

bool KVStore::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string tmp;
    return get(key, tmp);
}

int KVStore::size() {
    std::lock_guard<std::mutex> lock(mtx);
    return skipList->getSize();
}

void KVStore::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    skipList->clear();
    lruCache->clear();
}
