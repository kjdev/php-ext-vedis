<?php
if (!extension_loaded('vedis')) {
    dl('vedis.' . PHP_SHLIB_SUFFIX);
}
