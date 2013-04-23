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
  constructor->PrototypeTemplate()->Set(String::New("remove"), FunctionTemplate::New(Remove)->GetFunction());
  constructor->PrototypeTemplate()->Set(String::New("clear"), FunctionTemplate::New(Clear)->GetFunction());
  constructor->PrototypeTemplate()->Set(String::New("size"), FunctionTemplate::New(Size)->GetFunction());

  exports->Set(String::NewSymbol("HashTable"), Persistent<Function>::New(constructor->GetFunction()));
}

HashTable::HashTable() {}

HashTable::HashTable(size_t buckets) : map(buckets) {}

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

Handle<Value> HashTable::Get(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  //How the fuck do I throw exceptions from V8
  Local<Value> key = Local<Value>(args[0]);
  String::AsciiValue keyStr(key);

  MapType::const_iterator itr = obj->map.find(std::string(*keyStr));

  if(itr == obj->map.end()) {
    return scope.Close(Handle<Value>()); //return undefined
  } 

  Persistent<Value> value = itr->second;

  return scope.Close(value);
}

Handle<Value> HashTable::Put(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  Local<Value> key = Local<Value>(args[0]);
  Local<Value> value = Local<Value>(args[1]);

  String::AsciiValue keyStr(key);

  MapType::const_iterator itr = obj->map.find(std::string(*keyStr));

  //overwriting an existing value
  if(itr != obj->map.end()) {
    Persistent<Value> oldValue = itr->second;
    oldValue.Dispose(); //release the handle to the GC
  } 

  Persistent<Value> persistent = Persistent<Value>::New(value);

  obj->map.insert(std::pair<std::string, Persistent<Value>>(std::string(*keyStr), persistent));

  //Return undefined
  return scope.Close(Local<Value>());
}

Handle<Value> HashTable::Remove(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  Local<Value> key = Local<Value>(args[0]);
  String::AsciiValue keyStr(key);

  auto itr = obj->map.find(std::string(*keyStr));

  if(itr == obj->map.end()) {
    return scope.Close(Local<Value>()); //do nothing and return undefined
  } 

  Persistent<Value> value = itr->second;
  value.Dispose();

  obj->map.erase(itr);

  return scope.Close(Local<Value>());
}

Handle<Value> HashTable::Clear(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  for(auto itr = obj->map.begin(); itr != obj->map.end(); ) {
    Persistent<Value> value = itr->second;
    value.Dispose();

    itr = obj->map.erase(itr);
  }

  return scope.Close(Local<Value>());
}

Handle<Value> HashTable::Size(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  return scope.Close(Integer::New(obj->map.size()));
}

