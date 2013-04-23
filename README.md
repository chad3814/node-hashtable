HashTable
=========

Sometimes you need to store so much data in memory that V8 can get a bit clogged up. This Node.js module provides an interface to a native hashmap data structure that exists outside of V8's memory constraints.

To install, simply:

    npm install hashtable

Usage
-----

Everything you might want to do first requires a new HashMap object (which corresponds to a native c++ unordered_map):

    var HashTable = require('hashtable');
    var hashtable = new HashTable();
    
    hashtable.put('key', {value: 'value'});

    console.log(hashtable.get('value'));
    
    ...
    
    > { value: 'value' }
    
And that's it! Note that values can be any javascript type, including objects. The module properly creates and removes references (aka 'handles' if you know something about v8 internals) as needed, so you don't have to worry about any garbage collection funny business. Just use the module like you would any other javascript library.

The hash table implementation is provided by C++11's `unordered_map` class. Currently there is no fallback for older compilers. Pull requests welcome.
