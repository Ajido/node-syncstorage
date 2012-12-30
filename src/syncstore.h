#ifndef NODE_LEVELDB_SYNCDB_H_
#define NODE_LEVELDB_SYNCDB_H_

#include <leveldb/db.h>
#include <node.h>

using namespace v8;

namespace node_syncstore {

class SyncStore : public node::ObjectWrap {
 public:
  static void Init(Handle<Object> target);

 private:
  SyncStore(leveldb::DB*);
  ~SyncStore();

  void close();

  static Handle<Value> New(const Arguments& args);
  static Handle<Value> Set(const Arguments& args);
  static Handle<Value> Get(const Arguments& args);
  static Handle<Value> Del(const Arguments& args);
  static Handle<Value> Exists(const Arguments& args);
  static Handle<Value> Size(const Arguments& args);
  static Handle<Value> Close(const Arguments& args);
  static Handle<Value> Each(const Arguments& args);

  leveldb::DB* db_;
};

}

#endif
