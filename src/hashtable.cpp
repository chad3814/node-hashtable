#include "hashtable.h"
#include <iostream>
#include <unordered_map>

using namespace v8;

void HashTable::init(Handle<Object> exports) {
  Local<FunctionTemplate> constructor = FunctionTemplate::New(Constructor);
  constructor->SetClassName(String::NewSymbol("HashTable"));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);

  auto prototype = constructor->PrototypeTemplate();
  prototype->Set("put", FunctionTemplate::New(Put)->GetFunction());
  prototype->Set("get", FunctionTemplate::New(Get)->GetFunction());
  prototype->Set("keys", FunctionTemplate::New(Keys)->GetFunction());
  prototype->Set("remove", FunctionTemplate::New(Remove)->GetFunction());
  prototype->Set("clear", FunctionTemplate::New(Clear)->GetFunction());
  prototype->Set("size", FunctionTemplate::New(Size)->GetFunction());
  prototype->Set("rehash", FunctionTemplate::New(Rehash)->GetFunction());
  prototype->Set("reserve", FunctionTemplate::New(Reserve)->GetFunction());
  prototype->Set("max_load_factor", FunctionTemplate::New(MaxLoadFactor)->GetFunction());

  exports->Set(String::NewSymbol("HashTable"), Persistent<Function>::New(constructor->GetFunction()));
}

HashTable::HashTable() {}

HashTable::HashTable(size_t buckets) : map(buckets) {}

HashTable::~HashTable() {
  for(auto itr = this->map.begin(); itr != this->map.end(); ) {
    Persistent<Value> value = itr->second;
    value.Dispose();

    itr = this->map.erase(itr);
  }
}

Handle<Value> HashTable::Constructor(const Arguments& args) {
  HashTable *obj;

  if(args.Length() > 0 && args[0]->IsInt32()) {
    int buckets = args[0]->Int32Value();
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

  auto itr = obj->map.find(std::string(*keyStr));

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

  auto itr = obj->map.find(std::string(*keyStr));

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

Handle<Value> HashTable::Keys(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  Local<Array> array = Array::New();

  int i = 0;
  for(auto itr = obj->map.begin(); itr != obj->map.end(); ++itr, ++i) {
    array->Set(Integer::New(i), String::New(itr->first.c_str()));
  }

  return scope.Close(array);
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

Handle<Value> HashTable::Rehash(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  size_t buckets = args[0]->Int32Value();

  obj->map.rehash(buckets);

  return scope.Close(Local<Value>());
}

Handle<Value> HashTable::Reserve(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  size_t elements = args[0]->Int32Value();

  obj->map.rehash(elements);

  return scope.Close(Local<Value>());
}

Handle<Value> HashTable::MaxLoadFactor(const Arguments& args) {
  HandleScope scope;

  HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

  if(args.Length() > 0) {
    Number *num = static_cast<Number*>(*args[0]);
    float factor = (float)num->Value();
    if(factor > 0)
      obj->map.max_load_factor(factor);

    return scope.Close(Local<Value>());
  } else {
    float factor = obj->map.max_load_factor();

    return scope.Close(Number::New((double)factor));
  }
}
