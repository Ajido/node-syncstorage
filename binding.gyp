{
  "targets": [
    {
      "target_name": "syncstore",
      "sources": [
        "src/syncstore.cc",
        "src/syncstore.h",
        "src/binding.cc"
      ],
      "dependencies": [
        'node_modules/leveldb/deps/leveldb/leveldb.gyp:leveldb'
      ]
    }
  ]
}
