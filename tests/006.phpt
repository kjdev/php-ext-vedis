--TEST--
vedis mSet
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->mset(array('x' => 'a', 'y' => 'b', 'z' => 'c')));
var_dump($vedis->mget(array('x', 'y', 'z')));

$vedis->delete('x');  // delete just x

var_dump($vedis->mset(array('x' => 'A', 'y' => 'B', 'z' => 'C')));
var_dump($vedis->mget(array('x', 'y', 'z')));

var_dump($vedis->mset(array()));

$set_array = array(-1 => 'neg1', -2 => 'neg2', -3 => 'neg3',
                   1 => 'one', 2 => 'two', '3' => 'three');

var_dump($vedis->mset($set_array));
var_dump($vedis->mget(array_keys($set_array)));
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
bool(true)
array(3) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
}
bool(false)
bool(true)
array(6) {
  [0]=>
  string(4) "neg1"
  [1]=>
  string(4) "neg2"
  [2]=>
  string(4) "neg3"
  [3]=>
  string(3) "one"
  [4]=>
  string(3) "two"
  [5]=>
  string(5) "three"
}
