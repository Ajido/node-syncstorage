node-syncstore
==============
Persistent Synchronous Key-Value Store for Node.js  
[![Build Status](https://travis-ci.org/ajido/node-syncstore.png?branch=master)](https://travis-ci.org/ajido/node-syncstore)

## Installation
```bash
$ npm install syncstore
```

## Example
```javascript
var syncstore = require('syncstore');
var store = new syncstore.SyncStore('./cachedb');

store.set('key', 'value');
console.log(store.get('key')); // value
console.log(store.exists('key')); // true

store.each(function(index, key, value) {
  // ...
});

console.log(store.size()) // 1
store.del('key');

store.close();
```

--------------------------------------------------------

What you should know
====================
* SyncStore is depending on leveldb. leveldb is not multi-process safe.
* Don't use this for regular server code. It will ruin the responsiveness of your server.

--------------------------------------------------------
