--TEST--
vedis sSize
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->sAdd('set', 'val'));
var_dump($vedis->sSize('set'));
var_dump($vedis->sAdd('set', 'val2'));
var_dump($vedis->sSize('set'));

?>
--EXPECTF--
int(1)
int(1)
int(1)
int(2)
