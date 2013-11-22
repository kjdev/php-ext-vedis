--TEST--
vedis incr/incrBy
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->set('key', 0);

$vedis->incr('key');
var_dump((int)$vedis->get('key'));

$vedis->incr('key');
var_dump((int)$vedis->get('key'));

$vedis->incrBy('key', 3);
var_dump((int)$vedis->get('key'));

$vedis->incrBy('key', 1);
var_dump((int)$vedis->get('key'));

$vedis->incrBy('key', -1);
var_dump((int)$vedis->get('key'));

$vedis->delete('key');

$vedis->set('key', 'abc');

$vedis->incr('key');
var_dump($vedis->get('key'));

$vedis->incr('key');
var_dump($vedis->get('key'));
?>
--EXPECTF--
int(1)
int(2)
int(5)
int(6)
int(5)
string(1) "1"
string(1) "2"
