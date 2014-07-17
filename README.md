node-syncstorage
==============
Persistent Key-Value Store for Node.js. Not support asynchronous API.  
[![Build Status](https://travis-ci.org/Ajido/node-syncstorage.svg?branch=master)](https://travis-ci.org/Ajido/node-syncstorage)

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
* SyncStore is dependent on leveldb. It is not multi-process safe.
* Don't use this for regular server code. It will ruin the responsiveness of your server.

--------------------------------------------------------
