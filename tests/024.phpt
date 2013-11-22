--TEST--
vedis sDiff
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

$xy = $vedis->sDiff('x', 'y');    // x U y
foreach($xy as $i) {
    $i = (int)$i;
    var_dump(in_array($i, array_diff($x, $y)));
}

$yz = $vedis->sDiff('y', 'z');    // y U Z
foreach($yz as $i) {
    $i = (int)$i;
    var_dump(in_array($i, array_diff($y, $z)));
}

$zt = $vedis->sDiff('z', 't');    // z U t
foreach($zt as $i) {
    $i = (int)$i;
    var_dump(in_array($i, array_diff($z, $t)));
}

$xyz = $vedis->sDiff('x', 'y', 'z'); // x U y U z
foreach($xyz as $i) {
    $i = (int)$i;
    var_dump(in_array($i, array_diff($x, $y, $z)));
}
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
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
