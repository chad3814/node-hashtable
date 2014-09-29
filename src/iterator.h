#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H

#include <string>
#include <iostream>
#include <node.h>
#include "hashtable.h"

class PairNodeIterator : public node::ObjectWrap {
public:
    static void init();
    static v8::Local<v8::Object> New(int type, MapType::const_iterator new_iter, MapType::const_iterator new_end);

    const static int KEY_TYPE = 1;
    const static int VALUE_TYPE = 1 << 1;

private:
    PairNodeIterator(MapType::const_iterator new_iter, MapType::const_iterator new_end);
    //~PairNodeIterator();

    MapType::const_iterator iter;
    MapType::const_iterator end;

    // iterator.done : boolean
    static v8::Handle<v8::Value> GetDone(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.key : value
    static v8::Handle<v8::Value> GetKey(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.value : value
    static v8::Handle<v8::Value> GetValue(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.next() : undefined
    static v8::Handle<v8::Value> Next(const v8::Arguments &args);
};

#endif
