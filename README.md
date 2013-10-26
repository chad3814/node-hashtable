HashTable
=========

![](https://travis-ci.org/isaacbw/node-hashtable.png?branch=master)

Sometimes you need to store so much data in memory that V8 can get a bit clogged up. This Node.js module provides an interface to a native hashmap data structure that exists outside of V8's memory constraints.

To install, simply:

    npm install hashtable
    
"But Isaac, javascript already has hash tables! They're called objects, dummy"
---

V8 is great, but was never really meant for driving large software systems. Try adding a few million non-integer keys to an object and you'll start to see things bog down. This module is *not* intended to be a general replacement for javascript objects (that would be silly). Instead, it is meant to be used when you need maps larger than V8's virtual machine can handle.

Usage
-----

Everything you might want to do first requires a new HashMap object (which corresponds to a native c++ unordered_map):

    var HashTable = require('hashtable');
    var hashtable = new HashTable();
    
    hashtable.put('key', {value: 'value'});

    console.log(hashtable.get('key'));
    
    ...
    
    > { value: 'value' }
    
And that's it! Note that values can be any javascript type, including objects. The module properly creates and removes references (aka 'handles' if you know something about v8 internals) as needed, so you don't have to worry about any garbage collection funny business. Just use the module like you would any other javascript library.

The hash table implementation is provided by C++11's `unordered_map` class. Currently there is no fallback for older compilers. Pull requests welcome.

### `put ( key, value )`

Insert a new key/value pair in the hashmap. The key must be a string, and the value can be any javascript type, including objects.

### `get ( key )`

Lookup a value from its key. Will return undefined if the key does not exist.

### `remove ( key )`

Remove a key/value pair by its key. If the key does not exist, no action will be performed.

### `clear ()`

Removes all key/value pairs from the hash table.

### `size ()`

Returns the number of key/value pairs in the hash table.

### `rehash ( n )`

Will increase the number of buckets to at least `n`, possibly causing a rehash of the hash table. See [unordered_map#rehash](http://www.cplusplus.com/reference/unordered_map/unordered_map/rehash/)

### `reserve ( n )`

Gives a hint to the implementation which may cause a rehash to the most appropriate number of buckets to contain `n` key/value pairs. See [unordered_map#reserve](http://www.cplusplus.com/reference/unordered_map/unordered_map/reserve/)

### `max_load_factor ()` or `max_load_factor ( factor )`

Either returns or sets the max load factor of the hash table implementation. This value determines when the hash map is rehashed with a new bucket count. By default it is `1.0`. See [unordered_map#max_load_factor](http://www.cplusplus.com/reference/unordered_map/unordered_map/max_load_factor/)

