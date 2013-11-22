--TEST--
vedis hSetNx
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->hSetNx('h', 'x', 'a'));
var_dump($vedis->hSetNx('h', 'y', 'b'));
var_dump($vedis->hSetNx('h', 'x', '?'));
var_dump($vedis->hSetNx('h', 'y', '?'));
var_dump($vedis->hGet('h', 'x'));
var_dump($vedis->hGet('h', 'y'));

?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(false)
string(1) "a"
string(1) "b"
