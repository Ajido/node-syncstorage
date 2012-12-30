#define BUILDING_NODE_EXTENSION
#include <leveldb/db.h>
#include <node.h>
#include "syncstore.h"

using namespace v8;
using namespace node;

namespace node_syncstore {

SyncStore::SyncStore(leveldb::DB* db) : ObjectWrap(), db_(db) {
}

SyncStore::~SyncStore() {
  close();
}

void SyncStore::close() {
  if (db_ != NULL) delete db_;
}

void SyncStore::Init(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  t->SetClassName(String::NewSymbol("SyncStore"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  // TODO: repair
  t->PrototypeTemplate()->Set(String::NewSymbol("set"), FunctionTemplate::New(Set)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("get"), FunctionTemplate::New(Get)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("del"), FunctionTemplate::New(Del)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("exists"), FunctionTemplate::New(Exists)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("size"), FunctionTemplate::New(Size)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("close"), FunctionTemplate::New(Close)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("each"), FunctionTemplate::New(Each)->GetFunction());

  Persistent<Function> constructor = Persistent<Function>::New(t->GetFunction());
  target->Set(String::NewSymbol("SyncStore"), constructor);
}

Handle<Value> SyncStore::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 1 || !args[0]->IsString()) {
    return ThrowException(Exception::TypeError(String::New("Invalid arguments")));
  }

  // TODO: other options
  leveldb::Options options;
  options.create_if_missing = true;

  leveldb::DB* db = NULL;
  leveldb::Status s = leveldb::DB::Open(options, *String::Utf8Value(args[0]), &db);

  if (!s.ok()) {
    return ThrowException(Exception::Error(String::New("Failed to open store")));
  }

  SyncStore* self = new SyncStore(db);
  self->Wrap(args.This());

  return args.This();
}

Handle<Value> SyncStore::Set(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsString()) {
    return ThrowException(Exception::TypeError(String::New("Invalid arguments")));
  }

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());

  if (self->db_ == NULL) {
    return ThrowException(Exception::Error(String::New("Store is closed")));
  }

  std::string key = *String::Utf8Value(args[0]);
  std::string val = *String::Utf8Value(args[1]);

  // TODO: sync
  leveldb::WriteOptions write_options;
  leveldb::Status s = self->db_->Put(write_options, key, val);

  return scope.Close(Boolean::New(s.ok()));
}

Handle<Value> SyncStore::Get(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 1 || !args[0]->IsString()) {
    return ThrowException(Exception::TypeError(String::New("Invalid arguments")));
  }

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());
  if (self->db_ == NULL) {
    return ThrowException(Exception::Error(String::New("Store is closed")));
  }

  std::string key = *String::Utf8Value(args[0]);
  std::string value;

  // TODO: fill_cache, verify_checksums
  leveldb::ReadOptions read_options;
  leveldb::Status s = self->db_->Get(read_options, key, &value);

  return scope.Close(String::New(value.c_str()));
}

Handle<Value> SyncStore::Del(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 1 || !args[0]->IsString()) {
    return ThrowException(Exception::TypeError(String::New("Invalid arguments")));
  }

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());
  if (self->db_ == NULL) {
    return ThrowException(Exception::Error(String::New("Store is closed")));
  }

  std::string key = *String::Utf8Value(args[0]);

  // TODO: sync
  leveldb::WriteOptions write_options;
  leveldb::Status s = self->db_->Delete(write_options, key);

  return scope.Close(Boolean::New(s.ok()));
}

Handle<Value> SyncStore::Close(const Arguments& args) {
  HandleScope scope;

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());
  self->close();

  return scope.Close(Undefined());
}

Handle<Value> SyncStore::Exists(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 1 || !args[0]->IsString()) {
    return ThrowException(Exception::TypeError(String::New("Invalid arguments")));
  }

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());
  if (self->db_ == NULL) {
    return ThrowException(Exception::Error(String::New("Store is closed")));
  }

  std::string key = *String::Utf8Value(args[0]);
  std::string value;

  // TODO: fill_cache, verify_checksums
  leveldb::ReadOptions read_options;
  leveldb::Status s = self->db_->Get(read_options, key, &value);

  return scope.Close(Boolean::New(!s.IsNotFound()));
}

Handle<Value> SyncStore::Size(const Arguments& args) {
  HandleScope scope;

  unsigned long count = 0;

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());
  if (self->db_ == NULL) {
    return ThrowException(Exception::Error(String::New("Store is closed")));
  }

  leveldb::ReadOptions read_options;
  read_options.fill_cache = false;

  leveldb::Iterator* it = self->db_->NewIterator(read_options);

  for (it->SeekToFirst(); it->Valid(); it->Next()) count++;

  if (!(it->status().ok())) {
    return scope.Close(Number::New(-1));
  }

  delete it;

  return scope.Close(Number::New(count));
}

Handle<Value> SyncStore::Each(const Arguments& args) {
  HandleScope scope;

  unsigned long index = 0;
  const unsigned argc = 3;

  if (args.Length() != 1 || !args[0]->IsFunction()) {
    return ThrowException(Exception::TypeError(String::New("Invalid arguments")));
  }

  Local<Function> cb = Local<Function>::Cast(args[0]);

  SyncStore* self = ObjectWrap::Unwrap<SyncStore>(args.This());
  if (self->db_ == NULL) {
    return ThrowException(Exception::Error(String::New("Store is closed")));
  }

  leveldb::ReadOptions read_options;
  read_options.fill_cache = false;

  leveldb::Iterator* it = self->db_->NewIterator(read_options);

  for (it->SeekToFirst(); it->Valid(); it->Next()) {
      Local<Value> argv[argc] = {
          Local<Value>::New(Number::New(index)),
          Local<Value>::New(String::New(it->key().ToString().c_str())),
          Local<Value>::New(String::New(it->value().ToString().c_str()))
      };

      TryCatch tryCatch;
      cb->Call(Context::GetCurrent()->Global(), argc, argv);
      if (tryCatch.HasCaught()) FatalException(tryCatch);
      index++;
  }

  if (!(it->status().ok())) {
    return ThrowException(String::New(it->status().ToString().c_str()));
  }

  delete it;

  return scope.Close(Undefined());
}

}
