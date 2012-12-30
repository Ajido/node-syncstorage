build:
	if [ ! -d node_modules ]; then npm submodule leveldb@0.7.1; fi
	node-gyp configure build

clean: publish
	node-gyp clean

publish:
	if [ -d node_modules ]; then rm -rf node_modules; fi

test:
	node test/test.js

.PHONY: build clean test publish
