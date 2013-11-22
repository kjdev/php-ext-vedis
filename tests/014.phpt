--TEST--
vedis hSet/hGet/hLen
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

$vedis= new Vedis;

var_dump($vedis->hLen('h'));
var_dump($vedis->hSet('h', 'a', 'a-value'));
var_dump($vedis->hLen('h'));
var_dump($vedis->hSet('h', 'b', 'b-value'));
var_dump($vedis->hLen('h'));

var_dump($vedis->hGet('h', 'a'));  // simple get
var_dump($vedis->hGet('h', 'b'));  // simple get

var_dump($vedis->hSet('h', 'a', 'another-value')); // replacement
var_dump($vedis->hGet('h', 'a'));    // get the new value

var_dump($vedis->hGet('h', 'b'));  // simple get
var_dump($vedis->hGet('h', 'c'));  // unknown hash member
var_dump($vedis->hGet('key', 'c'));    // unknownkey
?>
--EXPECTF--
int(0)
bool(true)
int(1)
bool(true)
int(2)
string(7) "a-value"
string(7) "b-value"
bool(true)
string(13) "another-value"
string(7) "b-value"
NULL
NULL
