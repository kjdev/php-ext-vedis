--TEST--
vedis eval
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->get('set'));

var_dump($vedis->eval("SET set test; GET set"));
var_dump($vedis->get('set'));

var_dump($vedis->eval("DEL set; GET set;"));

?>
--EXPECTF--
NULL
string(4) "test"
string(4) "test"
string(0) ""
