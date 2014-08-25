'use strict';

var ES6Iterator = function (native_map, type) {
    var keys = [];
    var values = [];
    native_map.forEach(function (key, value) {
        keys.push(key);
        values.push(value);
    });

    var idx = 0;
    var iterator = Object.defineProperties({}, {
        done: {
            get: function () {
                return idx >= keys.length;
            },
            enumerable: true
        },
        next: {
            value: function () {
                idx += 1;
                return this;
            },
            enumerable: true
        }
    });

    if (type !== 'key' && type !== 'value') {
        type = 'key+value';
    }

    switch (type) {
    case 'key':
        Object.defineProperty(iterator, 'key', {
            get: function () {
                if (this.done) {
                    return undefined;
                }
                return keys[idx];
            },
            enumerable: true
        });
        break;
    case 'value':
        Object.defineProperty(iterator, 'value', {
            get: function () {
                if (this.done) {
                    return undefined;
                }
                return values[idx];
            },
            enumerable: true
        });
        break;
    case 'key+value':           // default
        Object.defineProperties(iterator, {
            key: {
                get: function () {
                    if (this.done) {
                        return undefined;
                    }
                    return keys[idx];
                },
                enumerable: true
            },
            value: {
                get: function () {
                    if (this.done) {
                        return undefined;
                    }
                    return values[idx];
                },
                enumerable: true
            }
        });
        break;
    }

    return iterator;
};

module.exports = ES6Iterator;
