--TEST--
vedis sRem
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->sAdd('set', 'val');
$vedis->sAdd('set', 'val2');

$vedis->sRem('set', 'val');

var_dump($vedis->sSize('set'));

$vedis->sRemove('set', 'val2');

var_dump($vedis->sSize('set'));

?>
--EXPECTF--
int(1)
int(0)
