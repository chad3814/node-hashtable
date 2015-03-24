#include "iterator.h"
#include <iostream>

using namespace v8;

#define PROTO(proto, js_name, cpp_name)    \
    (proto)->Set(NanNew<String>(#js_name), \
                 NanNew<FunctionTemplate>(cpp_name)->GetFunction())

static Persistent<FunctionTemplate> key_value_tmplt;
static Persistent<FunctionTemplate> key_tmplt;
static Persistent<FunctionTemplate> value_tmplt;

void PairNodeIterator::init() {
    Local<FunctionTemplate> tmplt = NanNew<FunctionTemplate>();
    tmplt->SetClassName(NanNew<String>("NodeIterator"));
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Local<ObjectTemplate> key_value_prototype = tmplt->PrototypeTemplate();
    NanAssignPersistent(key_value_tmplt, tmplt);

    tmplt = NanNew<FunctionTemplate>();
    tmplt->SetClassName(NanNew<String>("NodeIterator"));
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Local<ObjectTemplate> key_prototype = tmplt->PrototypeTemplate();
    NanAssignPersistent(key_tmplt, tmplt);

    tmplt = NanNew<FunctionTemplate>();
    tmplt->SetClassName(NanNew<String>("NodeIterator"));
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Local<ObjectTemplate> value_prototype = tmplt->PrototypeTemplate();
    NanAssignPersistent(value_tmplt, tmplt);

    PROTO(key_value_prototype, next, Next);
    PROTO(key_prototype, next, Next);
    PROTO(value_prototype, next, Next);
}

Local<Object> PairNodeIterator::New(int type, MapType::const_iterator new_iter, MapType::const_iterator new_end) {
    Handle<FunctionTemplate> tmplt;
    if ((PairNodeIterator::KEY_TYPE & type) && (PairNodeIterator::VALUE_TYPE & type)) {
        tmplt = NanNew<FunctionTemplate>(key_value_tmplt);
    } else if (KEY_TYPE & type) {
        tmplt = NanNew<FunctionTemplate>(key_tmplt);
    } else {
        tmplt = NanNew<FunctionTemplate>(value_tmplt);
    }

    Local<Object> obj = tmplt->InstanceTemplate()->NewInstance();
    PairNodeIterator *iter = new PairNodeIterator(new_iter, new_end);

    iter->Wrap(obj);

    Local<String> key = NanNew<String>("key");
    Local<String> value = NanNew<String>("value");
    Local<String> done = NanNew<String>("done");

    if (PairNodeIterator::KEY_TYPE & type) {
        obj->SetAccessor(key, GetKey);
    }
    if (PairNodeIterator::VALUE_TYPE & type) {
        obj->SetAccessor(value, GetValue);
    }

    obj->SetAccessor(done, GetDone);

    return obj;
}

PairNodeIterator::PairNodeIterator(MapType::const_iterator new_iter, MapType::const_iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
NAN_GETTER(PairNodeIterator::GetDone) {
    NanScope();

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(args.This());

    if (obj->iter == obj->end) {
        NanReturnValue(NanTrue());
    }
    NanReturnValue(NanFalse());
}


// iterator.key : value
NAN_GETTER(PairNodeIterator::GetKey) {
    NanScope();

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(args.This());

    if (obj->iter == obj->end) {
        NanReturnUndefined();
    }

    NanReturnValue(obj->iter->first->Extract());
}

// iterator.value : value
NAN_GETTER(PairNodeIterator::GetValue) {
    NanScope();

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator>(args.This());

    if (obj->iter == obj->end) {
        NanReturnUndefined();
    }

    NanReturnValue(obj->iter->second->Extract());
}

// iterator.next() : undefined
NAN_METHOD(PairNodeIterator::Next) {
    NanScope();

    PairNodeIterator *obj = ObjectWrap::Unwrap<PairNodeIterator >(args.This());

    if (obj->iter == obj->end) {
        NanReturnThis();
    }

    obj->iter++;

    NanReturnThis();
}
