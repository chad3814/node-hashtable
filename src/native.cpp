#include <node.h>
#include <v8.h>
#include "hashtable.h"

using namespace v8;

void init(Handle<Object> exports) {
  HashTable::init(exports);
}

NODE_MODULE(native, init);
