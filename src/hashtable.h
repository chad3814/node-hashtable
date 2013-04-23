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

  std::unordered_map<std::string, std::string> map;

  static v8::Handle<v8::Value> Constructor(const v8::Arguments &args);
  static v8::Handle<v8::Value> Get(const v8::Arguments &args);
  static v8::Handle<v8::Value> Put(const v8::Arguments &args);
  static v8::Handle<v8::Value> Size(const v8::Arguments &args);
};

#endif
