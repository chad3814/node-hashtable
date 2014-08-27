#include "iterator.h"
#include <iostream>

using namespace v8;

Local<Object> PairNodeIterator::init(int type, MapType::iterator new_iter, MapType::iterator new_end) {
    Local<ObjectTemplate> tmplt = ObjectTemplate::New();
    tmplt->SetInternalFieldCount(1);

    PairNodeIterator *iter = new PairNodeIterator(new_iter, new_end);

    if (PairNodeIterator::KEY_TYPE & type) {
        tmplt->SetAccessor(String::New("key"), GetKey);
    }

    if (PairNodeIterator::VALUE_TYPE & type) {
        tmplt->SetAccessor(String::New("value"), GetValue);
    }

    tmplt->SetAccessor(String::New("done"), GetDone);
    tmplt->Set(String::New("next"), FunctionTemplate::New(Next)->GetFunction());
    Local<Object> obj = tmplt->NewInstance();

    iter->Wrap(obj);
    return obj;
}

PairNodeIterator::PairNodeIterator(MapType::iterator new_iter, MapType::iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
Handle<Value> PairNodeIterator::GetDone(Local<String> property, const AccessorInfo &info) {
    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.Holder());

    return Boolean::New(obj->iter == obj->end);
}


// iterator.key : value
Handle<Value> PairNodeIterator::GetKey(Local<String> property, const AccessorInfo &info) {
    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.Holder());

    return obj->iter->first;
}

// iterator.value : value
Handle<Value> PairNodeIterator::GetValue(Local<String> property, const AccessorInfo &info) {
    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(info.Holder());

    return obj->iter->second;
}

// iterator.next() : undefined
v8::Handle<v8::Value> PairNodeIterator::Next(const v8::Arguments &args) {
    HandleScope scope;

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator >(args.This());

    obj->iter++;

    return scope.Close(Undefined());
}
