--TEST--
vedis setnx
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

if ($vedis->set('key', 42) !== true) {
    echo "FAILURE: ${key} -> ${value}\n";
}
var_dump($vedis->setnx('key', 'err'));
var_dump($vedis->get('key'));

$vedis->delete('key');

var_dump($vedis->setnx('key', '42'));
var_dump($vedis->get('key'));
?>
--EXPECTF--
bool(false)
string(2) "42"
bool(true)
string(2) "42"
