--TEST--
vedis append
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->set('key', 'val1');
var_dump($vedis->append('key', 'val2'));
var_dump($vedis->get('key'));

var_dump($vedis->append('keyNotExist', 'value'));
var_dump($vedis->get('keyNotExist'));

?>
--EXPECTF--
int(8)
string(8) "val1val2"
int(5)
string(5) "value"
