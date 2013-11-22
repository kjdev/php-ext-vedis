--TEST--
vedis lSize
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->lPush('list', 'val');
var_dump($vedis->lSize('list'));

$vedis->lPush('list', 'val2');
var_dump($vedis->lSize('list'));

var_dump($vedis->lPop('list'));
var_dump($vedis->lSize('list'));

var_dump($vedis->lPop('list'));
var_dump($vedis->lSize('list'));

var_dump($vedis->lPop('list'));
var_dump($vedis->lSize('list'));    // empty returns 0

var_dump($vedis->lSize('list1'));    // non-existent returns 0

$vedis->set('list', 'actually not a list');
var_dump($vedis->lSize('list'));// not a list returns FALSE

?>
--EXPECTF--
int(1)
int(2)
string(3) "val"
int(1)
string(4) "val2"
int(0)
NULL
int(0)
int(0)
int(0)
