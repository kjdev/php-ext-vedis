--TEST--
vedis strlen
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->set('key', 'val1');
var_dump($vedis->strlen('key'));

$vedis->append('key', 'val1');
var_dump($vedis->strlen('key'));

var_dump($vedis->strlen('keyNotExist'));

?>
--EXPECTF--
int(4)
int(8)
int(0)
