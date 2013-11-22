--TEST--
vedis hKeys/hVals/hGetAll
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->hSetNx('h', 'x', 'a');
$vedis->hSetNx('h', 'y', 'b');

// keys
var_dump($vedis->hKeys('h'));

// values
var_dump($vedis->hVals('h'));

// keys + values
var_dump($vedis->hGetAll('h'));
?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  string(1) "y"
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  ["x"]=>
  string(1) "a"
  ["y"]=>
  string(1) "b"
}
