--TEST--
vedis hExists
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->hSetNx('h', 'x', 'a');
$vedis->hSetNx('h', 'y', 'b');

var_dump($vedis->hExists('h', 'x'));
var_dump($vedis->hExists('h', 'y'));
var_dump($vedis->hExists('h', 'w'));

$vedis->hDel('h', 'x');

var_dump($vedis->hExists('h', 'x'));

?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(false)
