--TEST--
vedis sAdd
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->sAdd('set', 'val'));
var_dump($vedis->sAdd('set', 'val'));

var_dump($vedis->sIsMember('set', 'val'));
var_dump($vedis->sIsMember('set', 'val2'));

var_dump($vedis->sAdd('set', 'val2'));

var_dump($vedis->sIsMember('set', 'val2'));

?>
--EXPECTF--
int(1)
int(1)
bool(true)
bool(false)
int(1)
bool(true)
