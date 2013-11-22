--TEST--
phpinfo() displays vedis info
--SKIPIF--
--FILE--
<?php
include_once dirname(__FILE__) . '/init.inc';

phpinfo();
?>
--EXPECTF--
%a
vedis

Vedis support => enabled
Version => %d.%d.%d
Extension Version => %d.%d.%d
%a
