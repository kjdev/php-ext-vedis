--TEST--
vedis lPush/lPop
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->lPush('list', 'val'));
var_dump($vedis->lPush('list', 'val2'));
var_dump($vedis->lPush('list', 'val3'));

// 'list' = [ 'val', 'val2', 'val3']

var_dump($vedis->lPop('list'));
var_dump($vedis->lPop('list'));
var_dump($vedis->lPop('list'));
var_dump($vedis->lPop('list'));

// testing binary data

$vedis->delete('list');
var_dump($vedis->lPush('list', gzcompress('val1')));
var_dump($vedis->lPush('list', gzcompress('val2')));
var_dump($vedis->lPush('list', gzcompress('val3')));

var_dump(gzuncompress($vedis->lPop('list')));
var_dump(gzuncompress($vedis->lPop('list')));
var_dump(gzuncompress($vedis->lPop('list')));

?>
--EXPECTF--
int(1)
int(2)
int(3)
string(3) "val"
string(4) "val2"
string(4) "val3"
NULL
int(1)
int(2)
int(3)
string(4) "val1"
string(4) "val2"
string(4) "val3"
