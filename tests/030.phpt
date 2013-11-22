--TEST--
vedis sPeek/sTop
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->sAdd('set', 'val1');
$vedis->sAdd('set', 'val2');
$vedis->sAdd('set', 'val3');
$vedis->sAdd('set', 'val4');
$vedis->sAdd('set', 'val5');

var_dump($vedis->sPeek('set'));
var_dump($vedis->sTop('set'));

$vedis->sRemove('set', 'val1');
$vedis->sRemove('set', 'val5');

var_dump($vedis->sPeek('set'));
var_dump($vedis->sTop('set'));

?>
--EXPECTF--
string(4) "val5"
string(4) "val1"
string(4) "val4"
string(4) "val2"
