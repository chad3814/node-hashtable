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

typedef unordered_map<std::string,v8::Persistent<v8::Value> > MapType;

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

    // hashTable.get(key) : value
    static v8::Handle<v8::Value> Get(const v8::Arguments &args);

    // hashTable.put(key, value) : undefined
    static v8::Handle<v8::Value> Put(const v8::Arguments &args);

    // hashTable.keys() : []
    static v8::Handle<v8::Value> Keys(const v8::Arguments &args);

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
};

#endif
