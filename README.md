Hashmap
=======

Sometimes you need to store so much data in memory that V8 can get a bit clogged up. This Node.js module provides an interface to a native hashmap data structure that exists outside of V8's memory constraints.

To install, simply:

    npm install hashmap

Usage
-----

Everything you might want to do first requires a new HashMap object (which corresponds to a native c++ unordered_map):

    var HashMap = require('hashmap');
    var hashmap = new HashMap();
    
And that's it!
