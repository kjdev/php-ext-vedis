--TEST--
vedis mGet
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->set('k1', 'v1');
$vedis->set('k2', 'v2');
$vedis->set('k3', 'v3');
$vedis->set(1, 'test');

var_dump($vedis->mGet(array('k1')));
var_dump($vedis->mGet(array('k1', 'k3', 'NoKey')));
var_dump($vedis->mGet(array('k1', 'k2', 'k3')));
var_dump($vedis->mGet(array('k1', 'k2', 'k3')));

$vedis->set('k5', '$1111111111');
var_dump($vedis->mGet(array('k5')));

var_dump($vedis->mGet(array(1)));

$vedis->delete('k1');
$vedis->delete('k2');
$vedis->delete('k3');

$vedis->set('k1', gzcompress('v1'));
$vedis->set('k2', gzcompress('v2'));
$vedis->set('k3', gzcompress('v3'));

var_dump(array(gzcompress('v1'), gzcompress('v2'), gzcompress('v3')) ===
         $vedis->mGet(array('k1', 'k2', 'k3')));
var_dump(array(gzcompress('v1'), gzcompress('v2'), gzcompress('v3')) ===
         $vedis->mGet(array('k1', 'k2', 'k3')));
?>
--EXPECTF--
array(1) {
  [0]=>
  string(2) "v1"
}
array(3) {
  [0]=>
  string(2) "v1"
  [1]=>
  string(2) "v3"
  [2]=>
  NULL
}
array(3) {
  [0]=>
  string(2) "v1"
  [1]=>
  string(2) "v2"
  [2]=>
  string(2) "v3"
}
array(3) {
  [0]=>
  string(2) "v1"
  [1]=>
  string(2) "v2"
  [2]=>
  string(2) "v3"
}
array(1) {
  [0]=>
  string(11) "$1111111111"
}
array(1) {
  [0]=>
  string(4) "test"
}
bool(true)
bool(true)
