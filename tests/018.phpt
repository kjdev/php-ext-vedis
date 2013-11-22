--TEST--
vedis hMSet/hMGet
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

// hmset
var_dump($vedis->hMset('h', array('x' => 123, 'y' => 456, 'z' => 'abc')));
var_dump($vedis->hGet('h', 'x'));
var_dump($vedis->hGet('h', 'y'));
var_dump($vedis->hGet('h', 'z'));
var_dump($vedis->hGet('h', 't'));

// hmget
var_dump($vedis->hMget('h', array('x', 'y')));
var_dump($vedis->hMget('h', array('z')));
var_dump($vedis->hMget('h', array('x', 't', 'y')));
var_dump($vedis->hMget('h', array(123)));

// Test with an array populated with things we can't use as keys
var_dump($vedis->hmget('h', array(false, NULL, false))); //false

// Test with some invalid keys mixed in (which should just be ignored)
var_dump($vedis->hMget('h',Array('x',null,'y','','z',false)));

// hmget/hmset with numeric fields
$vedis->hDel('h', 'x');
$vedis->hDel('h', 'y');
$vedis->hDel('h', 'z');

var_dump($vedis->hMset('h', array(123 => 'x', 'y' => 456)));
var_dump($vedis->hGet('h', 123));
var_dump($vedis->hGet('h', '123'));
var_dump($vedis->hGet('h', 'y'));
var_dump($vedis->hMget('h', array('123', 'y')));
?>
--EXPECTF--
bool(true)
string(3) "123"
string(3) "456"
string(3) "abc"
NULL
array(2) {
  ["x"]=>
  string(3) "123"
  ["y"]=>
  string(3) "456"
}
array(1) {
  ["z"]=>
  string(3) "abc"
}
array(3) {
  ["x"]=>
  string(3) "123"
  ["t"]=>
  NULL
  ["y"]=>
  string(3) "456"
}
array(1) {
  [123]=>
  NULL
}
array(0) {
}
array(3) {
  ["x"]=>
  string(3) "123"
  ["y"]=>
  string(3) "456"
  ["z"]=>
  string(3) "abc"
}
bool(true)
string(1) "x"
string(1) "x"
string(3) "456"
array(2) {
  [123]=>
  string(1) "x"
  ["y"]=>
  string(3) "456"
}
