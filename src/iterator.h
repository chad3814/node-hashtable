#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H

#include <string>
#include <iostream>
#include <node.h>
#include <nan.h>
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
    static NAN_GETTER(GetDone);

    // iterator.key : value
    static NAN_GETTER(GetKey);

    // iterator.value : value
    static NAN_GETTER(GetValue);

    // iterator.next() : undefined
    static NAN_METHOD(Next);
};

#endif
