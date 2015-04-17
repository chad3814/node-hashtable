#include "hashtable.h"
#include <iostream>
#include "iterator.h"

using namespace v8;

#define PROTO(proto, js_name, cpp_name) \
    (proto)->Set(NanNew<String>(#js_name), \
                 NanNew<FunctionTemplate>(cpp_name)->GetFunction())


void HashTable::init(Handle<Object> exports) {
    Local<FunctionTemplate> ht_constructor = NanNew<FunctionTemplate>(Constructor);
    ht_constructor->SetClassName(NanNew<String>("HashTable"));
    ht_constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> ht_prototype = ht_constructor->PrototypeTemplate();

    PROTO(ht_prototype, put, Put);
    PROTO(ht_prototype, get, Get);
    PROTO(ht_prototype, has, Has);
    PROTO(ht_prototype, keys, Keys);
    PROTO(ht_prototype, remove, Remove);
    PROTO(ht_prototype, clear, Clear);
    PROTO(ht_prototype, size, Size);
    PROTO(ht_prototype, rehash, Rehash);
    PROTO(ht_prototype, reserve, Reserve);
    PROTO(ht_prototype, max_load_factor, MaxLoadFactor);
    PROTO(ht_prototype, forEach, ForEach);

    Local<FunctionTemplate> map_constructor = NanNew<FunctionTemplate>(Constructor);
    map_constructor->SetClassName(NanNew<String>("NodeMap"));
    map_constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> map_prototype = map_constructor->PrototypeTemplate();
    PROTO(map_prototype, set, Put);
    PROTO(map_prototype, get, Get);
    PROTO(map_prototype, has, Has);
    PROTO(map_prototype, keys, MapKeys);
    PROTO(map_prototype, values, MapValues);
    PROTO(map_prototype, entries, MapEntries);
    PROTO(map_prototype, delete, Remove);
    PROTO(map_prototype, clear, Clear);
    PROTO(map_prototype, rehash, Rehash);
    PROTO(map_prototype, reserve, Reserve);
    PROTO(map_prototype, max_load_factor, MaxLoadFactor);
    PROTO(map_prototype, forEach, MapForEach);

    exports->Set(NanNew<String>("HashTable"), ht_constructor->GetFunction());
    exports->Set(NanNew<String>("NodeMap"), map_constructor->GetFunction());

    PairNodeIterator::init();
}

HashTable::HashTable() {}

HashTable::HashTable(size_t buckets) : map(buckets) {}

HashTable::~HashTable() {
    for(MapType::const_iterator itr = this->map.begin(); itr != this->map.end(); ) {
        delete itr->first;
        delete itr->second;

        itr = this->map.erase(itr);
    }
}

NAN_METHOD(HashTable::Constructor) {
    NanScope();

    HashTable *obj;
    if(args.Length() > 0 && args[0]->IsInt32()) {
        int buckets = args[0]->Int32Value();
        obj = new HashTable(buckets);
    } else {
        obj = new HashTable();
    }

    obj->Wrap(args.This());

    NanReturnThis();
}

NAN_METHOD(HashTable::MapConstructor) {
    NanScope();

    HashTable *obj;

    if(args.Length() > 0 && args[0]->IsInt32()) {
        int buckets = args[0]->Int32Value();
        obj = new HashTable(buckets);
    } else {
        obj = new HashTable();
    }

    obj->Wrap(args.This());
    args.This()->SetAccessor(NanNew<String>("size"), MapSize);

    NanReturnThis();
}

NAN_METHOD(HashTable::Get) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
    V8PersistentValueWrapper *persistent = new V8PersistentValueWrapper(Isolate::GetCurrent(), args[0]);

    MapType::const_iterator itr = obj->map.find(persistent);

    if(itr == obj->map.end()) {
        NanReturnUndefined();
    }

    NanReturnValue(itr->second->Extract());
}

NAN_METHOD(HashTable::Has) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
    V8PersistentValueWrapper *persistent = new V8PersistentValueWrapper(Isolate::GetCurrent(), args[0]);

    MapType::const_iterator itr = obj->map.find(persistent);

    if(itr == obj->map.end()) {
        NanReturnValue(NanFalse());
    }

    NanReturnValue(NanTrue());
}

NAN_METHOD(HashTable::Put) {
    NanScope();

    if (args.Length() < 2 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
    V8PersistentValueWrapper *pkey = new V8PersistentValueWrapper(Isolate::GetCurrent(), args[0]);
    V8PersistentValueWrapper *pvalue = new V8PersistentValueWrapper(Isolate::GetCurrent(), args[1]);

    MapType::const_iterator itr = obj->map.find(pkey);

    //overwriting an existing value
    if(itr != obj->map.end()) {
        delete itr->first;
        delete itr->second;

        obj->map.erase(itr);
    }

    obj->map.insert(std::pair<V8PersistentValueWrapper *, V8PersistentValueWrapper *>(pkey, pvalue));

    //Return this
    NanReturnThis();
}

NAN_METHOD(HashTable::Keys) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Array> array = NanNew<Array>();

    int i = 0;
    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ++itr, ++i) {
        array->Set(NanNew<Integer>(i), itr->first->Extract());
    }

    NanReturnValue(array);
}

NAN_METHOD(HashTable::MapEntries) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE | PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    NanReturnValue(iter);
}

NAN_METHOD(HashTable::MapKeys) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::KEY_TYPE, obj->map.begin(), obj->map.end());

    NanReturnValue(iter);
}

NAN_METHOD(HashTable::MapValues) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    Local<Object> iter = PairNodeIterator::New(PairNodeIterator::VALUE_TYPE, obj->map.begin(), obj->map.end());

    NanReturnValue(iter);
}


NAN_METHOD(HashTable::Remove) {
    NanScope();

    if (args.Length() < 1) {
        NanThrowTypeError("Wrong arguments");
        NanReturnValue(NanFalse());
    }

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
    V8PersistentValueWrapper *persistent = new V8PersistentValueWrapper(Isolate::GetCurrent(), args[0]);

    MapType::const_iterator itr = obj->map.find(persistent);

    if(itr == obj->map.end()) {
        //do nothing and return false
        NanReturnValue(NanFalse());
    }

    delete itr->first;
    delete itr->second;

    obj->map.erase(itr);

    NanReturnValue(NanTrue());
}

NAN_METHOD(HashTable::Clear) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
    for(MapType::const_iterator itr = obj->map.begin(); itr != obj->map.end(); ) {
        delete itr->first;
        delete itr->second;

        itr = obj->map.erase(itr);
    }

    NanReturnUndefined();
}

NAN_METHOD(HashTable::Size) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());
    uint32_t size = obj->map.size();

    NanReturnValue(NanNew<Integer>(size));
}

NAN_GETTER(HashTable::MapSize) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    uint32_t size = obj->map.size();

    NanReturnValue(NanNew<Integer>(size));
}

NAN_METHOD(HashTable::Rehash) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    size_t buckets = args[0]->Int32Value();

    obj->map.rehash(buckets);

    NanReturnUndefined();
}

NAN_METHOD(HashTable::Reserve) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    size_t elements = args[0]->Int32Value();

    obj->map.rehash(elements);

    NanReturnUndefined();
}

NAN_METHOD(HashTable::MaxLoadFactor) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    double old_factor = obj->map.max_load_factor();
    double factor;

    if(args.Length() > 0) {
        factor = args[0]->NumberValue();
        if(factor > 0) {
            obj->map.max_load_factor(factor);
        }
    }

    NanReturnValue(NanNew<Number>(old_factor));
}

NAN_METHOD(HashTable::ForEach) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = NanGetCurrentContext()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = args.This();

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = itr->first->Extract();
        argv[1] = itr->second->Extract();
        cb->Call(ctx, argc, argv);
        itr++;
    }

    NanReturnUndefined();
}

NAN_METHOD(HashTable::MapForEach) {
    NanScope();

    HashTable *obj = ObjectWrap::Unwrap<HashTable>(args.This());

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = NanGetCurrentContext()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = args.This();

    MapType::const_iterator itr = obj->map.begin();

    while (itr != obj->map.end()) {
        argv[0] = itr->second->Extract();
        argv[1] = itr->first->Extract();
        cb->Call(ctx, argc, argv);
        itr++;
    }

    NanReturnUndefined();
}
