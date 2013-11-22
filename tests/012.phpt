--TEST--
vedis exists
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->delete('key');
var_dump($vedis->exists('key'));
$vedis->set('key', 'val');
var_dump($vedis->exists('key'));
?>
--EXPECTF--
bool(false)
bool(true)
