--TEST--
vedis set
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis);

if ($vedis->set('key', 'nil') !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', 'val') !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));
var_dump($vedis->get('key'));

$vedis->delete('keyNotExist');
var_dump($vedis->get('keyNotExist'));

if ($vedis->set('key2', 'val') !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key2'));

$value = 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA';

if ($vedis->set('key2', $value) !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key2'));
var_dump($vedis->get('key2'));

$vedis->delete('key');
$vedis->delete('key2');

$i = 66000;
$value2 = 'X';
while ($i--) {
    $value2 .= 'A';
}
$value2 .= 'X';

if ($vedis->set('key', $value2) !== true) {
    echo "FAILURE\n";
}
if (strcmp($vedis->get('key'), $value2) == 0) {
    var_dump(true);
}
$vedis->delete('key');
var_dump($vedis->get('key'));

$data = gzcompress('42');
if ($vedis->set('key', $data) !== true) {
    echo "FAILURE\n";
}
var_dump(gzuncompress($vedis->get('key')));

$vedis->delete('key');
$data = gzcompress('value1');
if ($vedis->set('key', $data) !== true) {
    echo "FAILURE\n";
}
var_dump(gzuncompress($vedis->get('key')));

$vedis->delete('key');

if ($vedis->set('key', 0) !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', 1) !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', 0.1) !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', '0.1') !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', true) !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', '') !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));

if ($vedis->set('key', null) !== true) {
    echo "FAILURE\n";
}
var_dump($vedis->get('key'));


if ($vedis->set('key', gzcompress('42')) !== true) {
    echo "FAILURE\n";
}
var_dump(gzuncompress($vedis->get('key')));
?>
--EXPECTF--
object(Vedis)#%d (0) {
}
string(3) "nil"
string(3) "val"
string(3) "val"
NULL
string(3) "val"
string(112) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
string(112) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
bool(true)
NULL
string(2) "42"
string(6) "value1"
string(1) "0"
string(1) "1"
string(3) "0.1"
string(3) "0.1"
string(1) "1"
string(0) ""
string(0) ""
string(2) "42"
