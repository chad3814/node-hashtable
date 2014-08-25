'use strict';

var HashTable = require('./');
var makeIterator = require('./es6-iterator');

var ES6Map = function (iterator) {
    this._native_map = new HashTable();
    if (iterator && iterator.hasOwnProperty('done') && iterator.hasOwnProperty('next') && iterator.hasOwnProperty('key') && iterator.hasOwnProperty('value')) {
        while (!iterator.done) {
            this._native_map.put(iterator.key, iterator.value);
            iterator.next();
        }
    }
};

ES6Map.prototype = {
    get: function (key) {
        return this._native_map.get(key);
    },
    has: function (key) {
        return this._native_map.get(key) !== undefined;
    },
    set: function (key, value) {
        return this._native_map.put(key, value);
    },
    delete: function (key) {
        return this._native_map.remove(key);
    },
    forEach: function (cb, ctx) {
        return this._native_map.forEach(function (key, value) {
            return cb.call(ctx, value, key, this);
        }, this);
    },
    clear: function () {
        return this._native_map.clear();
    },
    keys: function () {
        return makeIterator(this._native_map, 'key');
    },
    values: function () {
        return makeIterator(this._native_map, 'value');
    },
    entries: function () {
        return makeIterator(this._native_map, 'key+value');
    }
};

Object.defineProperty(ES6Map.prototype, 'size', {
    get: function () {
        return this._native_map.size();
    },
    enumerable: true,
    configurable: true
});

module.exports = ES6Map;
