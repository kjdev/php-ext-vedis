--TEST--
vedis del
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$key = 'key' . rand();
$vedis->set($key, 'val');
var_dump($vedis->get($key));
var_dump($vedis->delete($key));
var_dump($vedis->get($key));

// multiple, all existing
$vedis->set('x', 0);
$vedis->set('y', 1);
$vedis->set('z', 2);
var_dump($vedis->delete('x', 'y', 'z'));
var_dump($vedis->get('x'));
var_dump($vedis->get('y'));
var_dump($vedis->get('z'));

// multiple, none existing
var_dump($vedis->delete('x', 'y', 'z'));
var_dump($vedis->get('x'));
var_dump($vedis->get('y'));
var_dump($vedis->get('z'));

 // multiple, some existing
$vedis->set('y', 1);
var_dump($vedis->delete('x', 'y', 'z'));
var_dump($vedis->get('y'));
?>
--EXPECTF--
string(3) "val"
int(1)
NULL
int(3)
NULL
NULL
NULL
int(0)
NULL
NULL
NULL
int(1)
NULL
