#include "hashtable.h"
#include <iostream>
#include <unordered_map>

using namespace v8;

void HashTable::init(Handle<Object> exports) {
  Local<FunctionTemplate> constructor = FunctionTemplate::New(Constructor);
  constructor->SetClassName(String::NewSymbol("HashTable"));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->PrototypeTemplate()->Set(String::New("put"), FunctionTemplate::New(Put)->GetFunction());
  constructor->PrototypeTemplate()->Set(String::New("get"), FunctionTemplate::New(Get)->GetFunction());
  constructor->PrototypeTemplate()->Set(String::New("size"), FunctionTemplate::New(Size)->GetFunction());

  exports->Set(String::NewSymbol("HashTable"), Persistent<Function>::New(constructor->GetFunction()));
}

HashTable::HashTable() {}

HashTable::HashTable(size_t buckets) : map(buckets) {
}

HashTable::~HashTable() {}

Handle<Value> HashTable::Constructor(const Arguments& args) {
  HashTable *obj;

  if(args.Length() > 0 && (*args[0])->IsInt32()) {
    int buckets = (*args[0])->Int32Value();
    std::cout << "HashTable(" << buckets << ")" << std::endl;
    obj = new HashTable(buckets);
  } else {
    obj = new HashTable();
  }

  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> HashTable::Put(const Arguments& args) {
  HandleScope scope;

  Local<Value> key = Local<Value>(args[0]);
  Local<Value> value = Local<Value>(args[1]);

  String::AsciiValue keyStr(key);
  String::AsciiValue valueStr(value);

  //Persistent<Value> persistent = Persistent<Value>::New(value);

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
  //obj->map.insert(std::pair<std::string, Persistent<Value>>(std::string(*ascii), persistent));
  obj->map.insert(std::pair<std::string, std::string>(std::string(*keyStr), std::string(*valueStr)));

  //Return undefined
  return scope.Close(Handle<Value>());
}

Handle<Value> HashTable::Get(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  //How the fuck do I throw exceptions from V8
  Local<Value> key = Local<Value>(args[0]);
  String::AsciiValue keyStr(key);

  //std::unordered_map<std::string,Persistent<Value>>::const_iterator itr = obj->map.find(std::string(*keyAscii));
  std::unordered_map<std::string,std::string>::const_iterator itr = obj->map.find(std::string(*keyStr));

  if(itr == obj->map.end()) {
    return Handle<Value>(); //undefined
  } 

  /*Persistent<Value> persistent = itr->second;
  String::AsciiValue persistentAscii(persistent);
  std::cout << "key: '" << *keyAscii << "' value: '" << *persistentAscii << "'" << std::endl;*/
  //Handle<Value> value = itr->second;

  std::string str = itr->second;
  Handle<Value> value = String::New(str.c_str());

  return scope.Close(value);
}

Handle<Value> HashTable::Size(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  return scope.Close(Integer::New(obj->map.size()));
}

