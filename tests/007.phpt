--TEST--
vedis mSetNx
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->msetnx(array('x' => 'a', 'y' => 'b', 'z' => 'c')));

var_dump($vedis->mget(array('x', 'y', 'z')));

$vedis->delete('x');  // delete just x

var_dump($vedis->msetnx(array('x' => 'A', 'y' => 'B', 'z' => 'C')));
var_dump($vedis->mget(array('x', 'y', 'z')));

var_dump($vedis->msetnx(array()));
?>
--EXPECTF--
bool(true)
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
bool(false)
array(3) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
bool(false)
