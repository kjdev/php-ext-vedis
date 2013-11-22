--TEST--
vedis sIsMember
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->sAdd('set', 'val');

var_dump($vedis->sIsMember('set', 'val'));
var_dump($vedis->sIsMember('set', 'val2'));

?>
--EXPECTF--
bool(true)
bool(false)