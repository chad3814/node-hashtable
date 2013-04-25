var HashMap = require('..'),
    test = require('test-more')();

var hashmap = new HashMap();

test.ok(hashmap, 'new HashMap()')
    .is(function() {
        hashmap.max_load_factor(0.75);
        return hashmap.max_load_factor();
    }, 0.75, 'get/set max_load_factor')
    .test("object identity", function(test) {
        var obj = { hello: 'world' };
        hashmap.put('obj', obj);

        test.is(hashmap.get('obj'), obj)
            .isnt(hashmap.get('obj'), { hello: 'world' })
            .done();
    })
   .done();


