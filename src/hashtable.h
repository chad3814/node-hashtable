#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <node.h>
#include <string>
#ifdef __APPLE__
#include <tr1/unordered_map>
#define unordered_map std::tr1::unordered_map
#else
#include <unordered_map>
#define unordered_map std::unordered_map
#endif
#include "v8_value_hasher.h"

typedef unordered_map<v8::Persistent<v8::Value>,v8::Persistent<v8::Value>, v8_value_hash, v8_value_equal_to> MapType;

class HashTable : public node::ObjectWrap {
public:
    static void init(v8::Handle<v8::Object> exports);

private:
    HashTable();
    HashTable(size_t buckets);
    ~HashTable();

    MapType map;

    // new HashTable() or new HashTable(buckets)
    static v8::Handle<v8::Value> Constructor(const v8::Arguments &args);

    // new HashMap() or new HashMap(buckets)
    static v8::Handle<v8::Value> MapConstructor(const v8::Arguments &args);

    // hashTable.get(key) : value
    static v8::Handle<v8::Value> Get(const v8::Arguments &args);

    // hashTable.has(key) : boolean
    static v8::Handle<v8::Value> Has(const v8::Arguments &args);

    // hashTable.put(key, value) : hashtable
    static v8::Handle<v8::Value> Put(const v8::Arguments &args);

    // hashTable.keys() : []
    static v8::Handle<v8::Value> Keys(const v8::Arguments &args);

    // map.entries() : iterator
    static v8::Handle<v8::Value> MapEntries(const v8::Arguments &args);

    // map.keys() : iterator
    static v8::Handle<v8::Value> MapKeys(const v8::Arguments &args);

    // map.values() : iterator
    static v8::Handle<v8::Value> MapValues(const v8::Arguments &args);

    // map.size : number of elements
    static v8::Handle<v8::Value> MapSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // hashTable.remove(key) : boolean
    static v8::Handle<v8::Value> Remove(const v8::Arguments &args);

    // hashTable.clear() : undefined
    static v8::Handle<v8::Value> Clear(const v8::Arguments &args);

    // hashTable.size() : number of elements
    static v8::Handle<v8::Value> Size(const v8::Arguments &args);

    // hashTable.rehash(buckets) : undefined
    static v8::Handle<v8::Value> Rehash(const v8::Arguments &args);

    // hashTable.reserve(size) : undefined
    static v8::Handle<v8::Value> Reserve(const v8::Arguments &args);

    // hashTable.max_load_factor() or hashTable.max_load_factor(factor) : current or old max_load_factor
    static v8::Handle<v8::Value> MaxLoadFactor(const v8::Arguments &args);

    // hashTable.forEach(function (key, value) {...}, context) : undefined
    static v8::Handle<v8::Value> ForEach(const v8::Arguments &args);

    // hashTable.forEach(function (key, value) {...}, context) : undefined
    static v8::Handle<v8::Value> MapForEach(const v8::Arguments &args);
};

#endif
