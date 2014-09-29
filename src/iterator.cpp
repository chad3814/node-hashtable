#include "iterator.h"
#include <iostream>

using namespace v8;
static Persistent<FunctionTemplate> key_value_tmplt;
static Persistent<FunctionTemplate> key_tmplt;
static Persistent<FunctionTemplate> value_tmplt;

void PairNodeIterator::init() {
    key_value_tmplt = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
    key_tmplt = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
    value_tmplt = Persistent<FunctionTemplate>::New(FunctionTemplate::New());

    Handle<ObjectTemplate> key_value_obj_tmplt = key_value_tmplt->InstanceTemplate();
    Handle<ObjectTemplate> key_obj_tmplt = key_tmplt->InstanceTemplate();
    Handle<ObjectTemplate> value_obj_tmplt = value_tmplt->InstanceTemplate();

    key_value_obj_tmplt->SetInternalFieldCount(1);
    key_obj_tmplt->SetInternalFieldCount(1);
    value_obj_tmplt->SetInternalFieldCount(1);

    key_value_obj_tmplt->SetAccessor(String::New("key"), GetKey);
    key_obj_tmplt->SetAccessor(String::New("key"), GetKey);

    key_value_obj_tmplt->SetAccessor(String::New("value"), GetValue);
    value_obj_tmplt->SetAccessor(String::New("value"), GetValue);

    key_value_obj_tmplt->SetAccessor(String::New("done"), GetDone);
    key_obj_tmplt->SetAccessor(String::New("done"), GetDone);
    value_obj_tmplt->SetAccessor(String::New("done"), GetDone);

    Local<ObjectTemplate> key_value_prototype = key_value_tmplt->PrototypeTemplate();
    Local<ObjectTemplate> key_prototype = key_tmplt->PrototypeTemplate();
    Local<ObjectTemplate> value_prototype = value_tmplt->PrototypeTemplate();

    key_value_prototype->Set(String::New("next"), FunctionTemplate::New(Next)->GetFunction());
    key_prototype->Set(String::New("next"), FunctionTemplate::New(Next)->GetFunction());
    value_prototype->Set(String::New("next"), FunctionTemplate::New(Next)->GetFunction());
}

Local<Object> PairNodeIterator::New(int type, MapType::const_iterator new_iter, MapType::const_iterator new_end) {
    HandleScope scope;

    Handle<FunctionTemplate> tmplt;
    if ((PairNodeIterator::KEY_TYPE & type) && (PairNodeIterator::VALUE_TYPE & type)) {
        tmplt = key_value_tmplt;
    } else if (KEY_TYPE & type) {
        tmplt = key_tmplt;
    } else {
        tmplt = value_tmplt;
    }

    Local<Object> obj = tmplt->InstanceTemplate()->NewInstance();
    PairNodeIterator *iter = new PairNodeIterator(new_iter, new_end);

    iter->Wrap(obj);

    return scope.Close(obj);
}

PairNodeIterator::PairNodeIterator(MapType::const_iterator new_iter, MapType::const_iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
Handle<Value> PairNodeIterator::GetDone(Local<String> property, const AccessorInfo &info) {
    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.Holder());

    return Boolean::New(obj->iter == obj->end);
}


// iterator.key : value
Handle<Value> PairNodeIterator::GetKey(Local<String> property, const AccessorInfo &info) {
    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.Holder());

    if (obj->iter == obj->end) {
        return Undefined();
    }

    return obj->iter->first;
}

// iterator.value : value
Handle<Value> PairNodeIterator::GetValue(Local<String> property, const AccessorInfo &info) {
    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.Holder());

    if (obj->iter == obj->end) {
        return Undefined();
    }

    return obj->iter->second;
}

// iterator.next() : undefined
v8::Handle<v8::Value> PairNodeIterator::Next(const v8::Arguments &args) {
    HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator >(args.This());

    if (obj->iter == obj->end) {
        return scope.Close(args.This());
    }

    obj->iter++;

    return scope.Close(args.This());
}
