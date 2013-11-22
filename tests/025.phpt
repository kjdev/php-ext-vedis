--TEST--
vedis sInter
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

$x = array(1,3,5,7,9,11,13,15,17,19,21,23,25);
foreach($x as $i) {
    $vedis->sAdd('x', $i);
}

$y = array(1,2,3,5,7,11,13,17,19,23);
foreach($y as $i) {
    $vedis->sAdd('y', $i);
}

$z = array(1,4,9,16,25);
foreach($z as $i) {
    $vedis->sAdd('z', $i);
}

$t = array(2,5,10,17,26);
foreach($t as $i) {
    $vedis->sAdd('t', $i);
}

$xy = $vedis->sInter('x', 'y');   // odd prime numbers
foreach($xy as $i) {
    $i = (int)$i;
    var_dump(in_array($i, array_intersect($x, $y)));
}

$yz = $vedis->sInter('y', 'z');   // set of odd squares
foreach($yz as $i) {
    $i = (int)$i;
    var_dump(in_array($i, array_intersect($y, $z)));
}

$zt = $vedis->sInter('z', 't');   // prime squares
var_dump($zt === array());

$xyz = $vedis->sInter('x', 'y', 'z');// odd prime squares
var_dump($xyz === array('1'));

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
