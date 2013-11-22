--TEST--
vedis sMembers
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$vedis->sAdd('set', 'val');
$vedis->sAdd('set', 'val2');
$vedis->sAdd('set', 'val3');

$array = array('val', 'val2', 'val3');

$sMembers = $vedis->sMembers('set');
sort($sMembers);
var_dump($sMembers);

var_dump($vedis->sMembers('set1'));

?>
--EXPECTF--
array(3) {
  [0]=>
  string(3) "val"
  [1]=>
  string(4) "val2"
  [2]=>
  string(4) "val3"
}
array(0) {
}
