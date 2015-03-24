#ifndef V8_VALUE_HASHER_H
#define V8_VALUE_HASHER_H

#include <string>
#include <iostream>
#include <node.h>
#include <nan.h>
#ifdef __APPLE__
#include <tr1/unordered_set>
#define hash std::tr1::hash
#else
#include <unordered_set>
#define hash std::hash
#endif

// Node 0.11+ and io.js
class V8PersistentValueWrapper {
public:
    V8PersistentValueWrapper(v8::Isolate *isolate, v8::Local<v8::Value> value) : _isolate(isolate), _value(isolate, value) {}

    ~V8PersistentValueWrapper() {
        Dispose();
    }

    V8PersistentValueWrapper(V8PersistentValueWrapper const& other) {
        _value = other._value;
        _isolate = other._isolate;
    }

    V8PersistentValueWrapper& operator=(V8PersistentValueWrapper const& other) {
        if (this == &other) {
            return *this;
        }
        _value = other._value;
        _isolate = other._isolate;
        return *this;
    }

    v8::Local<v8::Value> Extract() {
        v8::EscapableHandleScope scope(_isolate);
        return scope.Escape(v8::Local<v8::Value>::New(_isolate, _value));
    }

    void Dispose() {
        _isolate = NULL;
        _value.Reset();
    }

private:

    v8::Isolate *_isolate;
    v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value> > _value;
};


struct v8_value_hash
{
    size_t operator()(V8PersistentValueWrapper *pkey) const {
        v8::Local<v8::Value> key = pkey->Extract();
        std::string s;
        if (key->IsString() || key->IsBoolean() || key->IsNumber()) {
            return hash<std::string>()(*NanUtf8String(key->ToString()));
        }
        return hash<int>()(key.As<v8::Object>()->GetIdentityHash());
    }
};

struct v8_value_equal_to
{
    bool operator()(V8PersistentValueWrapper *pa, V8PersistentValueWrapper *pb) const {
        v8::Local<v8::Value> a = pa->Extract();
        v8::Local<v8::Value> b = pb->Extract();

        if (a->Equals(b)) {          /* same as JS == */
            return true;
        }

        /* try basic types, if it is one, then can't be equal */
        if (a->IsString() || b->IsString()) {
            return false;
        }
        if (a->IsNumber() || b->IsNumber()) {
            return false;
        }
        if (a->IsBoolean() || b->IsBoolean()) {
            return false;
        }

        // if the identity hashes are equal, then it's equal
        return a.As<v8::Object>()->GetIdentityHash() == b.As<v8::Object>()->GetIdentityHash();
    }
};

#endif
