--TEST--
vedis sPop
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->sPop('set0'));

$vedis->sAdd('set0', 'val');
$vedis->sAdd('set0', 'val2');

var_dump($vedis->sPop('set0'));
var_dump($vedis->sSize('set0'));

var_dump($vedis->sPop('set0'));
var_dump($vedis->sSize('set0'));

var_dump($vedis->sPop('set0'));

?>
--EXPECTF--
NULL
string(4) "val2"
int(1)
string(3) "val"
int(0)
NULL
