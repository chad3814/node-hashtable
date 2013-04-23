#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <node.h>
#include <unordered_map>

class HashTable : public node::ObjectWrap {
 public:
  static void init(v8::Handle<v8::Object> exports);

 private:
  HashTable();
  HashTable(size_t buckets);
  ~HashTable();

  typedef std::unordered_map<std::string,v8::Persistent<v8::Value>> MapType;
  MapType map;


  //new HashTable() or new HashTable(buckets)
  static v8::Handle<v8::Value> Constructor(const v8::Arguments &args);

  //hashTable[key] : value
  static v8::Handle<v8::Value> GetNamedProperty(v8::Local<v8::String> key, const v8::AccessorInfo &info);

  //hashTable[key] = value
  static v8::Handle<v8::Value> SetNamedProperty(v8::Local<v8::String> key, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

  //hashTable.get(key) : value
  static v8::Handle<v8::Value> Get(const v8::Arguments &args);

  //hashTable.put(key, value)
  static v8::Handle<v8::Value> Put(const v8::Arguments &args);

  //hashTable.remove(key)
  static v8::Handle<v8::Value> Remove(const v8::Arguments &args);

  //hashTable.clear()
  static v8::Handle<v8::Value> Clear(const v8::Arguments &args);

  //hashTable.size() : number of elements
  static v8::Handle<v8::Value> Size(const v8::Arguments &args);

  //hashTable.rehash(buckets)
  static v8::Handle<v8::Value> Rehash(const v8::Arguments &args);

  //hashTable.reserve(size)
  static v8::Handle<v8::Value> Reserve(const v8::Arguments &args);

  //hashTable.max_load_factor() or hashTable.max_load_factor(factor)
  static v8::Handle<v8::Value> MaxLoadFactor(const v8::Arguments &args);
};

#endif
