--TEST--
vedis rename
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->delete('key0');
$vedis->set('key0', 'val0');
$vedis->rename('key0', 'key1');
var_dump($vedis->get('key0'));
var_dump($vedis->get('key1'));
?>
--EXPECTF--
NULL
string(4) "val0"
