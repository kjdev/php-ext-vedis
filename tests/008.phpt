--TEST--
vedis getSet
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->getSet('key', '42'));
var_dump($vedis->getSet('key', '123'));
var_dump($vedis->getSet('key', '123'));

?>
--EXPECTF--
NULL
string(2) "42"
string(3) "123"
