#include "hashtable.h"
#include <iostream>
#include "iterator.h"

using namespace v8;

void HashTable::init(Handle<Object> exports) {
    Local<FunctionTemplate> ht_constructor = FunctionTemplate::New(Constructor);
    ht_constructor->SetClassName(String::NewSymbol("HashTable"));
    ht_constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<FunctionTemplate> map_constructor = FunctionTemplate::New(MapConstructor);
    map_constructor->SetClassName(String::NewSymbol("NodeMap"));
    map_constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> ht_prototype = ht_constructor->PrototypeTemplate();
    ht_prototype->Set("put", FunctionTemplate::New(Put)->GetFunction());
    ht_prototype->Set("get", FunctionTemplate::New(Get)->GetFunction());
    ht_prototype->Set("has", FunctionTemplate::New(Has)->GetFunction());
    ht_prototype->Set("keys", FunctionTemplate::New(Keys)->GetFunction());
    ht_prototype->Set("remove", FunctionTemplate::New(Remove)->GetFunction());
    ht_prototype->Set("clear", FunctionTemplate::New(Clear)->GetFunction());
    ht_prototype->Set("size", FunctionTemplate::New(Size)->GetFunction());
    ht_prototype->Set("rehash", FunctionTemplate::New(Rehash)->GetFunction());
    ht_prototype->Set("reserve", FunctionTemplate::New(Reserve)->GetFunction());
    ht_prototype->Set("max_load_factor", FunctionTemplate::New(MaxLoadFactor)->GetFunction());
    ht_prototype->Set("forEach", FunctionTemplate::New(ForEach)->GetFunction());

    Local<ObjectTemplate> map_prototype = map_constructor->PrototypeTemplate();
    map_prototype->Set("set", FunctionTemplate::New(Put)->GetFunction());
    map_prototype->Set("get", FunctionTemplate::New(Get)->GetFunction());
    map_prototype->Set("has", FunctionTemplate::New(Has)->GetFunction());
    map_prototype->Set("keys", FunctionTemplate::New(MapKeys)->GetFunction());
    map_prototype->Set("values", FunctionTemplate::New(MapValues)->GetFunction());
    map_prototype->Set("entries", FunctionTemplate::New(MapEntries)->GetFunction());
    map_prototype->Set("delete", FunctionTemplate::New(Remove)->GetFunction());
    map_prototype->Set("clear", FunctionTemplate::New(Clear)->GetFunction());
    map_prototype->Set("rehash", FunctionTemplate::New(Rehash)->GetFunction());
    map_prototype->Set("reserve", FunctionTemplate::New(Reserve)->GetFunction());
    map_prototype->Set("max_load_factor", FunctionTemplate::New(MaxLoadFactor)->GetFunction());
    map_prototype->Set("forEach", FunctionTemplate::New(MapForEach)->GetFunction());

    exports->Set(String::NewSymbol("HashTable"), Persistent<Function>::New(ht_constructor->GetFunction()));
    exports->Set(String::NewSymbol("NodeMap"), Persistent<Function>::New(map_constructor->GetFunction()));

    PairNodeIterator::init();
}

HashTable::HashTable() {}

HashTable::HashTable(size_t buckets) : map(buckets) {}

HashTable::~HashTable() {
    for(MapType::const_iterator itr = this->map.begin(); itr != this->map.end(); ) {
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

Handle<Value> HashTable::MapConstructor(const Arguments& args) {
    HashTable *obj;

    if(args.Length() > 0 && args[0]->IsInt32()) {
        int buckets = args[0]->Int32Value();
        obj = new HashTable(buckets);
    } else {
        obj = new HashTable();
    }

    args.This()->SetAccessor(String::New("size"), MapSize);
    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> HashTable::Get(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Persistent<Value> key = Persistent<Value>(args[0]);

    MapType::const_iterator itr = obj->map.find(key);

    if(itr == obj->map.end()) {
        return scope.Close(Undefined()); //return undefined
    }

    Persistent<Value> value = itr->second;

    return scope.Close(value);
}

Handle<Value> HashTable::Has(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Persistent<Value> key = Persistent<Value>(args[0]);

    MapType::const_iterator itr = obj->map.find(key);

    if(itr == obj->map.end()) {
        return scope.Close(False()); //return undefined
    }

    return scope.Close(True());
}

Handle<Value> HashTable::Put(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Persistent<Value> key = Persistent<Value>::New(args[0]);
    Persistent<Value> value = Persistent<Value>::New(args[1]);

    MapType::const_iterator itr = obj->map.find(key);

    //overwriting an existing value
    if(itr != obj->map.end()) {
        Persistent<Value> oldKey = itr->first;
        oldKey.Dispose();
        oldKey.Clear();

        Persistent<Value> oldValue = itr->second;
        oldValue.Dispose(); //release the handle to the GC
        oldValue.Clear();

        obj->map.erase(itr);
    }

    obj->map.insert(std::pair<Persistent<Value>, Persistent<Value> >(key, value));

    //Return this
    return scope.Close(args.This());
}

Handle<Value> HashTable::Keys(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Array> array = Array::New();

    int i = 0;
    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ++itr, ++i) {
        array->Set(Integer::New(i), itr->first);
    }

    return scope.Close(array);
}

Handle<Value> HashTable::MapEntries(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE | PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    return scope.Close(iter);
}

Handle<Value> HashTable::MapKeys(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE, obj->map.begin(), obj->map.end());

    return scope.Close(iter);
}

Handle<Value> HashTable::MapValues(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    return scope.Close(iter);
}


Handle<Value> HashTable::Remove(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(False());
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Persistent<Value> key = Persistent<Value>(args[0]);

    MapType::const_iterator itr = obj->map.find(key);

    if(itr == obj->map.end()) {
        return scope.Close(False()); //do nothing and return false
    }

    Persistent<Value> stored_key = itr->first;
    stored_key.Dispose();
    stored_key.Clear();

    Persistent<Value> value = itr->second;
    value.Dispose();
    value.Clear();

    obj->map.erase(itr);

    return scope.Close(True());
}

Handle<Value> HashTable::Clear(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ) {
        Persistent<Value> key = itr->first;
        key.Dispose();
        key.Clear();

        Persistent<Value> value = itr->second;
        value.Dispose();
        value.Clear();

        itr = obj->map.erase(itr);
    }

    return scope.Close(Undefined());
}

Handle<Value> HashTable::Size(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    return scope.Close(Integer::New(obj->map.size()));
}

Handle<Value> HashTable::MapSize(Local<String> property, const AccessorInfo &info) {
    HashTable *obj = ObjectWrap::Unwrap<HashTable>(info.Holder());

    return Integer::New(obj->map.size());
}

Handle<Value> HashTable::Rehash(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    size_t buckets = args[0]->Int32Value();

    obj->map.rehash(buckets);

    return scope.Close(Undefined());
}

Handle<Value> HashTable::Reserve(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    size_t elements = args[0]->Int32Value();

    obj->map.rehash(elements);

    return scope.Close(Undefined());
}

Handle<Value> HashTable::MaxLoadFactor(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    float old_factor = obj->map.max_load_factor();

    if(args.Length() > 0) {
        Number *num = static_cast<Number*>(*args[0]);
        float factor = (float)num->Value();
        if(factor > 0)
            obj->map.max_load_factor(factor);

    }

    return scope.Close(Number::New((double)old_factor));
}

Handle<Value> HashTable::ForEach(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = Context::GetCurrent()->Global();
    }

    const unsigned argc = 2;
    Persistent<Value> argv[argc];

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = itr->first;
        argv[1] = itr->second;
        cb->Call(ctx, argc, argv);
        itr++;
    }

    return scope.Close(Undefined());
}

Handle<Value> HashTable::MapForEach(const Arguments& args) {
    HandleScope scope;

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = Context::GetCurrent()->Global();
    }

    const unsigned argc = 2;
    Persistent<Value> argv[argc];

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = itr->second;
        argv[1] = itr->first;
        cb->Call(ctx, argc, argv);
        itr++;
    }

    return scope.Close(Undefined());
}
