--TEST--
vedis lGet
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->lPush('list', 'val');
$vedis->lPush('list', 'val2');
$vedis->lPush('list', 'val3');

var_dump($vedis->lGet('list', 0));
var_dump($vedis->lGet('list', 1));
var_dump($vedis->lGet('list', 2));
var_dump($vedis->lGet('list', -1));
var_dump($vedis->lGet('list', -2));
var_dump($vedis->lGet('list', -3));
var_dump($vedis->lGet('list', -4));

$vedis->lPush('list', 'val4');
var_dump($vedis->lGet('list', 3));
var_dump($vedis->lGet('list', -1));
?>
--EXPECTF--
string(3) "val"
string(4) "val2"
string(4) "val3"
string(4) "val3"
string(4) "val2"
string(3) "val"
NULL
string(4) "val4"
string(4) "val4"
