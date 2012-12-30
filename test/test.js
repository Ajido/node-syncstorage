var assert = require('assert');
var path = require('path');
var exec = require('child_process').exec;

var dbpath1 = path.join(__dirname, 'testdb1');
var dbpath2 = path.join(__dirname, 'testdb2');

exec('rm -rf ' + dbpath1, function(err, strout, stderr) {
    var syncstore = require('../build/Release/syncstore');
    var store = new syncstore.SyncStore(dbpath1);
    var store2 = new syncstore.SyncStore(dbpath2);

    assert.ok(store.set('key1', '1'));
    assert.ok(store.set('key2', '2'));
    assert.ok(store.set('key3', '3'));

    assert.ok(store.get('key1') == '1');
    assert.ok(store.get('key2') == '2');
    assert.ok(store.get('key3') == '3');

    assert.ok(store.del('key2'));

    assert.ok(store2.set('key1', '_1'));
    assert.ok(store2.set('key2', '_2'));
    assert.ok(store2.set('key3', '_3'));

    assert.ok(store.exists('key1') == true);
    assert.ok(store.exists('key2') == false);
    assert.ok(store.exists('key3') == true);
    assert.ok(store.exists('key4') == false);

    assert.ok(store.get('key1') == '1');
    assert.ok(store.get('key2') == false);
    assert.ok(store.get('key3') == '3');

    assert.ok(store.set('key2', '4'));
    assert.ok(store.set('key3', '5'));

    assert.ok(store2.get('key1') == '_1');
    assert.ok(store2.get('key2') == '_2');
    assert.ok(store2.get('key3') == '_3');

    assert.ok(store.get('key1') == '1');
    assert.ok(store.get('key2') == '4');
    assert.ok(store.get('key3') == '5');

    assert.ok(store.del('key1'));
    assert.ok(store.del('key1'));
    assert.ok(store.del('key2'));

    assert.ok(store.get('key1') == false);
    assert.ok(store.get('key2') == false);
    assert.ok(store.get('key3') == '5');

    store.close();

    store = new syncstore.SyncStore(dbpath1);

    assert.ok(store.set('key1', '1'));
    assert.ok(store.set('key4', '6'));

    assert.ok(store.get('key1') == '1');
    assert.ok(store.get('key2') == false);
    assert.ok(store.get('key3') == '5');
    assert.ok(store.get('key4') == '6');

    store.each(function(index, key, value) {
        switch (index) {
            case 0:
                assert.ok(key == 'key1');
                assert.ok(value == '1');
                break;
            case 1:
                assert.ok(key == 'key3');
                assert.ok(value == '5');
                break;
            case 2:
                assert.ok(key == 'key4');
                assert.ok(value == '6');
                break;
            default:
                throw new Error();
        }
    });

    assert.ok(store.size() == 3);
    store.close();
    store2.close();
});

