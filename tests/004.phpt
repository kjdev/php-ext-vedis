--TEST--
vedis decr/decrBy
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->set('key', 5);

$vedis->decr('key');
var_dump((int)$vedis->get('key'));

$vedis->decr('key');
var_dump((int)$vedis->get('key'));

$vedis->decrBy('key', 2);
var_dump((int)$vedis->get('key'));

$vedis->decrBy('key', 1);
var_dump((int)$vedis->get('key'));

$vedis->decrBy('key', -10);
var_dump((int)$vedis->get('key'));
?>
--EXPECTF--
int(4)
int(3)
int(1)
int(0)
int(10)
