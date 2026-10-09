#include "lru_cache.h"

LRUCache::LRUCache(int cap) : capacity(cap), count(0) {
    head = new CacheNode();
    tail = new CacheNode();
    head->next = tail;
    tail->prev = head;
}

LRUCache::~LRUCache() {
    clear();
    delete head;
    delete tail;
}

void LRUCache::removeNode(CacheNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCache::addToHead(CacheNode* node) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void LRUCache::moveToHead(CacheNode* node) {
    removeNode(node);
    addToHead(node);
}

CacheNode* LRUCache::removeTail() {
    CacheNode* delNode = tail->prev;
    removeNode(delNode);
    return delNode;
}

bool LRUCache::get(const std::string& key, std::string& val) {
    auto it = hashMap.find(key);
    if(it == hashMap.end()) return false;
    CacheNode* node = it->second;
    val = node->value;
    moveToHead(node);
    return true;
}

void LRUCache::put(const std::string& key, const std::string& val) {
    auto it = hashMap.find(key);
    if(it != hashMap.end()) {
        CacheNode* node = it->second;
        node->value = val;
        moveToHead(node);
    } else {
        CacheNode* newNode = new CacheNode(key, val);
        hashMap[key] = newNode;
        addToHead(newNode);
        count++;

        if(count > capacity) {
            CacheNode* delNode = removeTail();
            hashMap.erase(delNode->key);
            delete delNode;
            count--;
        }
    }
}

bool LRUCache::del(const std::string& key) {
    auto it = hashMap.find(key);
    if(it == hashMap.end()) return false;
    CacheNode* node = it->second;
    removeNode(node);
    hashMap.erase(it);
    delete node;
    count--;
    return true;
}

void LRUCache::clear() {
    CacheNode* cur = head->next;
    while(cur != tail) {
        CacheNode* del = cur;
        cur = cur->next;
        delete del;
    }
    hashMap.clear();
    head->next = tail;
    tail->prev = head;
    count = 0;
}
