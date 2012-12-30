#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "syncstore.h"

using namespace v8;

namespace node_syncstore { extern "C" {

static void InitAll(Handle<Object> target) {
  SyncStore::Init(target);
}

NODE_MODULE(syncstore, InitAll);

}}
