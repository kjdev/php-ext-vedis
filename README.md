# PHP Extension for Vedis

This extension allows Vedis.

Documentation for Vedis can be found at [» http://vedis.symisc.net/](http://vedis.symisc.net/).

## Build

```
% phpize
% ./configure
% make
% make install
```

## Configration

vedis.ini:

```
extension=vedis.so
```

## Examples

```
$vedis = new Vedis;
$vedis->set('key', 'value');
$value = $vedis->get('key');
```

## Keys and Strings

### Strings

* [get](#get) - Get the value of a key
* [set](#set) - Set the string value of a key
* [setnx](#setnx) - Set the value of a key, only if the key does not exist
* [incr, incrBy](#incr-incrby) - Increment the value of a key
* [decr, decrBy](#decr-decrby) - Decrement the value of a key
* [mGet](#mget) - Get the values of all the given keys
* [mSet, mSetNX](#mset-msetnx) - Set multiple keys to multiple values
* [getSet](#getset) - Set the string value of a key and return its old value
* [append](#append) - Append a value to a key
* [strlen](#strlen) - Get the length of the value stored in a key

### Keys

* [del, delete](#del-delete) - Delete a key
* [exists](#exists) - Determine if a key exists
* [rename](#rename) - Rename a key

----
### get

_**Description**_: Get the value related to the specified key

#### *Parameters*

*key*

#### *Return value*

*String* or *NULL*: If key didn't exist, `NULL` is returned. Otherwise,
the value related to this key is returned.

#### *Examples*

```
$vedis->get('key');
```

----
### set

_**Description**_: Set the string value in argument as value of the key.

#### *Parameters*

*key*
*value*

#### *Return value*

*Bool* `TRUE` if the command is successful.

#### *Examples*

```
$vedis->set('key', 'value');
```

----
### setnx

_**Description**_: Set the string value in argument as value of the key if the
key doesn't already exist in the database.

#### *Parameters*

*key*
*value*

#### *Return value*

*Bool* `TRUE` in case of success, `FALSE` in case of failure.

#### *Examples*

```
$vedis->setnx('key', 'value'); /* TRUE */
$vedis->setnx('key', 'value'); /* FALSE */
```

----
### del, delete

_**Description**_: Remove specified keys.

#### *Parameters*

*keys*: key1, key2, ... , keyN: Any number of parameters, each a key.

#### *Return value*

*Long* Number of keys deleted.

#### *Examples*

```
$vedis->set('key1', 'val1');
$vedis->set('key2', 'val2');
$vedis->set('key3', 'val3');
$vedis->set('key4', 'val4');

$vedis->delete('key1', 'key2'); /* return 2 */
$vedis->delete('key4', 'key10'); /* return 1 */
```

----
### exists

_**Description**_: Verify if the specified key exists.

#### *Parameters*

*key*

#### *Return value*

*BOOL*: If the key exists, return `TRUE`, otherwise return `FALSE`.

#### *Examples*

```
$vedis->set('key', 'value');
$vedis->exists('key'); /* TRUE */
$vedis->exists('NonExistingKey'); /* FALSE */
```

----
### incr, incrBy

_**Description**_: Increment the number stored at key by one. If the second
argument is filled, it will be used as the integer value of the increment.

#### *Parameters*

*key*
*value*: value that will be added to key (only for incrBy)

#### *Return value*

*INT* the new value

#### *Examples*

```
$vedis->incr('key1'); /* key1 didn't exists, set to 0 before the increment */
                      /* and now has the value 1  */

$vedis->incr('key1'); /* 2 */
$vedis->incr('key1'); /* 3 */
$vedis->incr('key1'); /* 4 */
$vedis->incrBy('key1', 10); /* 14 */
```

----
### decr, decrBy

_**Description**_: Decrement the number stored at key by one. If the second
argument is filled, it will be used as the integer value of the decrement.

#### *Parameters*

*key*
*value*: value that will be substracted to key (only for decrBy)

#### *Return value*

*INT* the new value

#### *Examples*

```
$vedis->decr('key1'); /* key1 didn't exists, set to 0 before the increment */
                      /* and now has the value -1  */

$vedis->decr('key1'); /* -2 */
$vedis->decr('key1'); /* -3 */
$vedis->decrBy('key1', 10); /* -13 */
```

----
### mGet

_**Description**_: Get the values of all the specified keys. If one or more keys
dont exist, the array will contain `NULL` at the position of the key.

#### *Parameters*

*Array*: Array containing the list of the keys

#### *Return value*

*Array*: Array containing the values related to keys in argument

#### *Examples*

```
$vedis->set('key1', 'value1');
$vedis->set('key2', 'value2');
$vedis->set('key3', 'value3');
$vedis->mGet(array('key1', 'key2', 'key3')); /* array('value1', 'value2', 'value3'); */
$vedis->mGet(array('key0', 'key1', 'key5')); /* array(NULL, 'value2', NULL); */
```

----
### mset, msetnx

_**Description**_: Sets multiple key-value pairs in one atomic command. MSETNX
only returns TRUE if all the keys were set (see SETNX).

#### *Parameters*

*Pairs*: array(key => value, ...)

#### *Return value*

*Bool* `TRUE` in case of success, `FALSE` in case of failure.

#### *Example*

```
$vedis->mset(array('key0' => 'value0', 'key1' => 'value1')); /* TRUE */
$vedis->msetnx(array('key0' => 'value10', 'key1' => 'value11')); /* FALSE */

var_dump($vedis->get('key0'));
var_dump($vedis->get('key1'));
```

Output:

```
string(6) "value0"
string(6) "value1"
```

----
### getSet

_**Description**_: Sets a value and returns the previous entry at that key.

#### *Parameters*

*key*: key

*STRING*: value

#### *Return value*

A string, the previous value located at this key.

#### *Example*

```
$vedis->set('x', '42');
$vedis->getSet('x', 'lol'); /* 42, replaces x by 'lol' */
$vedis->get('x'); /* lol */
```

----
### rename

_**Description**_: Renames a key.

#### *Parameters*

*STRING*: srckey, the key to rename.

*STRING*: dstkey, the new name for the key.

#### *Return value*

*BOOL*: `TRUE` in case of success, `FALSE` in case of failure.

#### *Example*

```
$vedis->set('x', '42');
$vedis->rename('x', 'y');
$vedis->get('y'); /* 42 */
$vedis->get('x'); /* NULL */
```

----
### append

_**Description**_: Append specified string to the string stored in specified
key.

#### *Parameters*

*key*
*value*

#### *Return value*

*INTEGER*: Size of the value after the append

#### *Example*

```
$vedis->set('key', 'value1');
$vedis->append('key', 'value2'); /* 12 */
$vedis->get('key'); /* value1value2 */
```

----
### strlen

_**Description**_: Get the length of a string value.

#### *Parameters*

*key*

#### *Return value*

*INTEGER*

#### *Example*

```
$vedis->set('key', 'value');
$vedis->strlen('key'); /* 5 */
```


## Hashes

* [hSet](#hset) - Set the string value of a hash field
* [hSetNx](#hsetnx) - Set the value of a hash field, only if the field does not exist
* [hGet](#hget) - Get the value of a hash field
* [hLen](#hlen) - Get the number of fields in a hash
* [hDel](#hdel) - Delete one or more hash fields
* [hKeys](#hkeys) - Get all the fields in a hash
* [hVals](#hvals) - Get all the values in a hash
* [hGetAll](#hgetall) - Get all the fields and values in a hash
* [hExists](#hexists) - Determine if a hash field exists
* [hMSet](#hmset) - Set multiple hash fields to multiple values
* [hMGet](#hmget) - Get the values of all the given hash fields

----
### hSet

_**Description**_: Adds a value to the hash stored at key. If this value is
already in the hash, `FALSE` is returned.

#### *Parameters*

*key*
*field*
*value*

#### *Return value*

*BOOL* `TRUE` if the field was set, `FALSE` if it was already present.

#### *Example*

```
$vedis->hSet('h', 'key1', 'hello'); /* 'key1' => 'hello' in the hash at "h" */
$vedis->hGet('h', 'key1'); /* hello */

$vedis->hSet('h', 'key1', 'plop'); /* value was replaced. */
$vedis->hGet('h', 'key1'); /* returns "plop" */
```

----
### hSetNx

_**Description**_: Adds a value to the hash stored at key only if this field
isn't already in the hash.

#### *Return value*

*BOOL* `TRUE` if the field was set, `FALSE` if it was already present.

#### *Example*

```
$vedis->hSetNx('h', 'key1', 'hello'); /* TRUE, 'key1' => 'hello' in the hash at "h" */
$vedis->hSetNx('h', 'key1', 'world'); /* FALSE, 'key1' => 'hello' in the hash at "h". No change since the field wasn't replaced. */
```

----
### hGet

_**Description**_: Gets a value from the hash stored at key. If the hash table
doesn't exist, or the key doesn't exist, `FALSE` is returned.

#### *Parameters*

*key*
*field*

#### *Return value*

*STRING* The value, if the command executed successfully
*BOOL* `FALSE` in case of failure

#### *Examples*

```
$vedis->hGet('h', 'key');
```

----
### hLen

_**Description**_: Returns the length of a hash, in number of items

#### *Parameters*

*key*

#### *Return value*

*LONG* the number of items in a hash, `FALSE` if the key doesn't exist or isn't
a hash.

#### *Example*

```
$vedis->hSet('h', 'key1', 'hello');
$vedis->hSet('h', 'key2', 'plop');
$vedis->hLen('h'); /* 2 */
```

----
### hDel

_**Description**_: Removes a value from the hash stored at key. If the hash
table doesn't exist, or the key doesn't exist, `FALSE` is returned.

#### *Parameters*

*key*
*field*

#### *Return value*

*BOOL* `TRUE` in case of success, `FALSE` in case of failure

#### *Examples*

```
$vedis->hset('h', 'key1', 'val1');

$vedis->hdel('h', 'key1');
```

----
### hKeys

_**Description**_: Returns the keys in a hash, as an array of strings.

#### *Parameters*

*Key*: key

#### *Return value*

An array of elements, the keys of the hash. This works like PHP's array_keys().

#### *Example*

```
$vedis->hSet('h', 'a', 'x');
$vedis->hSet('h', 'b', 'y');
$vedis->hSet('h', 'c', 'z');
$vedis->hSet('h', 'd', 't');
var_dump($vedis->hKeys('h'));
```

Output:

```
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "d"
}
```

----
### hVals

_**Description**_: Returns the values in a hash, as an array of strings.

#### *Parameters*

*Key*: key

#### *Return value*

An array of elements, the values of the hash. This works like PHP's
array_values().

#### *Example*

```
$vedis->hSet('h', 'a', 'x');
$vedis->hSet('h', 'b', 'y');
$vedis->hSet('h', 'c', 'z');
$vedis->hSet('h', 'd', 't');
var_dump($vedis->hVals('h'));
```

Output:

```
array(4) {
  [0]=>
  string(1) "x"
  [1]=>
  string(1) "y"
  [2]=>
  string(1) "z"
  [3]=>
  string(1) "t"
}
```

----
### hGetAll

_**Description**_: Returns the whole hash, as an array of strings indexed by
strings.

#### *Parameters*

*Key*: key

#### *Return value*

An array of elements, the contents of the hash.

#### *Example*

```
$vedis->hSet('h', 'a', 'x');
$vedis->hSet('h', 'b', 'y');
$vedis->hSet('h', 'c', 'z');
$vedis->hSet('h', 'd', 't');
var_dump($vedis->hGetAll('h'));
```

Output:

```
array(4) {
  ["a"]=>
  string(1) "x"
  ["b"]=>
  string(1) "y"
  ["c"]=>
  string(1) "z"
  ["d"]=>
  string(1) "t"
}
```

----
## hExists

_**Description**_: Verify if the specified member exists in a key.

#### *Parameters*

*key*
*field*

#### *Return value*

*BOOL*: If the member exists in the hash table, return `TRUE`, otherwise return
`FALSE`.

#### *Examples*

```
$vedis->hSet('h', 'a', 'x');
$vedis->hExists('h', 'a'); /* TRUE */
$vedis->hExists('h', 'NonExistingKey'); /* FALSE */
```

----
### hMSet

_**Description**_: Fills in a whole hash. Non-string values are converted to
string, using the standard `(string)` cast. NULL values are stored as empty
strings.

#### *Parameters*

*key*
*members*: key -> value array

#### *Return value*

*BOOL*

#### *Examples*

```
$vedis->hMset('user:1', array('name' => 'Joe', 'salary' => 2000));
```

----
### hMGet

_**Description**_: Retrieve the values associated to the specified fields in the
hash.

#### *Parameters*

*key*
*memberKeys* Array

#### *Return value*

*Array* An array of elements, the values of the specified fields in the hash,
with the hash keys as array keys.

#### *Examples*

```
$vedis->hSet('h', 'field1', 'value1');
$vedis->hSet('h', 'field2', 'value2');
$vedis->hmGet('h', array('field1', 'field2')); /* array('field1' => 'value1', 'field2' => 'value2') */
```


## Lists

* [lIndex, lGet](#lindex-lget) - Get an element from a list by its index
* [lPop](#lpop) - Remove and get the first element in a list
* [lPush](#lpush) - Prepend one or multiple values to a list
* [lRem, lRemove](#lrem-lremove) - Remove elements from a list
* [lLen, lSize](#llen-lsize) - Get the length/size of a list

---
### lIndex, lGet

_**Description**_: Return the specified element of the list stored at the
specified key.

0 the first element, 1 the second ...
-1 the last element, -2 the penultimate ...

Return `FALSE` in case of a bad index or a key that doesn't point to a list.

#### *Parameters*

*key*
*index*

#### *Return value*

*String* the element at this index
*NULL* if the key identifies a non-string data type, or no value corresponds to
this index in the list `Key`.

#### *Example*

```
$vedis->lPush('key1', 'A');
$vedis->lPush('key1', 'B');
$vedis->lPush('key1', 'C'); /* key1 => [ 'A', 'B', 'C' ] */
$vedis->lGet('key1', 0); /* A */
$vedis->lGet('key1', -1); /* C */
$vedis->lGet('key1', 10); /* NULL */
```

----
### lPop

_**Description**_: Return and remove the first element of the list.

#### *Parameters*

*key*

#### *Return value*

*STRING* if command executed successfully
*BOOL* `FALSE` in case of failure (empty list)

#### *Example*

```
$vedis->lPush('key1', 'A');
$vedis->lPush('key1', 'B');
$vedis->lPush('key1', 'C'); /* key1 => [ 'A', 'B', 'C' ] */
$vedis->lPop('key1'); /* key1 => [ NULL, 'B', 'C' ] */
```

----
### lPush

_**Description**_: Adds the string value to the head (left) of the list. Creates
the list if the key didn't exist. If the key exists and is not a list, `FALSE`
is returned.

#### *Parameters*

*key*
*value* String, value to push in key

#### *Return value*

*LONG* The new length of the list in case of success, `FALSE` in case of Failure.

#### *Examples*

```
$vedis->lPush('key1', 'C'); /* 1 */
$vedis->lPush('key1', 'B'); /* 2 */
$vedis->lPush('key1', 'A'); /* 3 */
/* key1 now points to the following list: [ 'A', 'B', 'C' ] */
```

----
### lLen, lSize

_**Description**_: Returns the size of a list identified by Key.

If the list didn't exist or is empty, the command returns 0. If the data type
identified by Key is not a list, the command return `FALSE`.

#### *Parameters*

*key*

#### *Return value*

*LONG* The size of the list identified by Key exists.
*BOOL* `FALSE` if the data type identified by Key is not list

#### *Example*

```
$vedis->lPush('key1', 'A');
$vedis->lPush('key1', 'B');
$vedis->lPush('key1', 'C'); /* key1 => [ 'A', 'B', 'C' ] */
$vedis->lSize('key1');/* 3 */
$vedis->lPop('key1');
$vedis->lSize('key1');/* 2 */
```


## Sets

* [sAdd](#sadd) - Add one or more members to a set
* [sCard, sSize](#scard-ssize) - Get the number of members in a set
* [sDiff](#sdiff) - Subtract multiple sets
* [sInter](#sinter) - Intersect multiple sets
* [sIsMember](#sismember) - Determine if a given value is a member of a set
* [sMembers](#smembers) - Get all the members in a set
* [sPop](#spop) - Remove and get the last element in a set
* [sRem, sRemove](#srem-sremove) - Remove one or more members from a set
* [sPeek](#speek) - Get the last element in a set
* [sTop](#stop) - Get the first element in a set

---
### sAdd

_**Description**_: Adds a value to the set value stored at key. If this value is
already in the set, `FALSE` is returned.

#### *Parameters*

*key*
*value*

#### *Return value*

*LONG* the number of elements added to the set.

#### *Example*

```
$vedis->sAdd('key1' , 'member1'); /* 1, 'key1' => {'member1'} */
$vedis->sAdd('key1' , 'member2', 'member3'); /* 2, 'key1' => {'member1', 'member2', 'member3'}*/
$vedis->sAdd('key1' , 'member2'); /* 1, 'key1' => {'member1', 'member2', 'member3'}*/
```

----
### sCard, sSize

_**Description**_: Returns the cardinality of the set identified by key.

#### *Parameters*

*key*

#### *Return value*

*LONG* the cardinality of the set identified by key, 0 if the set doesn't exist.

#### *Example*

```
$vedis->sAdd('key1' , 'member1');
$vedis->sAdd('key1' , 'member2');
$vedis->sAdd('key1' , 'member3'); /* 'key1' => {'member1', 'member2', 'member3'}*/
$vedis->sCard('key1'); /* 3 */
$vedis->sCard('keyX'); /* 0 */
```

----
### sDiff

_**Description**_: Performs the difference between N sets and returns it.

#### *Parameters*

*keys*: key1, key2, ... , keyN: Any number of keys corresponding to sets.

#### *Return value*

*Array of strings*: The difference of the first set will all the others.

#### *Example*

```
$vedis->sAdd('s0', '1');
$vedis->sAdd('s0', '2');
$vedis->sAdd('s0', '3');
$vedis->sAdd('s0', '4');

$vedis->sAdd('s1', '1');
$vedis->sAdd('s2', '3');

var_dump($vedis->sDiff('s0', 's1', 's2'));
```

Return value: all elements of s0 that are neither in s1 nor in s2.

```
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "4"
}
```

----
### sInter

_**Description**_: Returns the members of a set resulting from the intersection
of all the sets held at the specified keys.

If just a single key is specified, then this command produces the members of
this set. If one of the keys is missing, `FALSE` is returned.

#### *Parameters*

key1, key2, keyN: keys identifying the different sets on which we will apply the
intersection.

#### *Return value*

Array, contain the result of the intersection between those keys. If the
intersection beteen the different sets is empty, the return value will be empty
array.

#### *Examples*

```
$vedis->sAdd('key1', 'val1');
$vedis->sAdd('key1', 'val2');
$vedis->sAdd('key1', 'val3');
$vedis->sAdd('key1', 'val4');

$vedis->sAdd('key2', 'val3');
$vedis->sAdd('key2', 'val4');

$vedis->sAdd('key3', 'val3');
$vedis->sAdd('key3', 'val4');

var_dump($vedis->sInter('key1', 'key2', 'key3'));
```

Output:

```
array(2) {
  [0]=>
  string(4) "val3"
  [1]=>
  string(4) "val4"
}
```

----
### sIsMember

_**Description**_: Checks if `value` is a member of the set stored at the key
`key`.

#### *Parameters*

*key*
*value*

#### *Return value*

*BOOL* `TRUE` if `value` is a member of the set at key `key`, `FALSE` otherwise.

#### *Example*

```
$vedis->sAdd('key1' , 'member1');
$vedis->sAdd('key1' , 'member2');
$vedis->sAdd('key1' , 'member3'); /* 'key1' => {'member1', 'member2', 'member3'}*/

$vedis->sIsMember('key1', 'member1'); /* TRUE */
$vedis->sIsMember('key1', 'memberX'); /* FALSE */
```

----
### sMembers

_**Description**_: Returns the contents of a set.

#### *Parameters*

*Key*: key

#### *Return value*

An array of elements, the contents of the set.

#### *Example*

```
$vedis->sAdd('s', 'a');
$vedis->sAdd('s', 'b');
$vedis->sAdd('s', 'a');
$vedis->sAdd('s', 'c');
var_dump($vedis->sMembers('s'));
```

Output:

```
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
```

----
### sPop

_**Description**_: Removes and returns a random element from the set value at
Key.

#### *Parameters*

*key*

#### *Return value*

*String* "popped" value
*Bool* `FALSE` if set identified by key is empty or doesn't exist.

#### *Example*

```
$vedis->sAdd('key1' , 'member1');
$vedis->sAdd('key1' , 'member2');
$vedis->sAdd('key1' , 'member3'); /* 'key1' => {'member1', 'member2', 'member3'}*/
$vedis->sPop('key1'); /* member3, 'key1' => {'member1', 'member2'} */
$vedis->sPop('key1'); /* member2, 'key1' => {'member1'} */
```

----
### sRem, sRemove

_**Description**_: Removes the specified member from the set value stored at key.

#### *Parameters*

*key*
*member*

#### *Return value*

*LONG* The number of elements removed from the set.

#### *Example*

```
$vedis->sAdd('key1' , 'member1');
$vedis->sAdd('key1' , 'member2');
$vedis->sAdd('key1' , 'member3'); /* 'key1' => {'member1', 'member2', 'member3'}*/
$vedis->sRem('key1', 'member2', 'member3'); /* 2. 'key1' => {'member1'} */
```

----
### sPeek

_**Description**_: Get the last element in a set

#### *Parameters*

*key*

#### *Return value*

*String* "popped" value
*Bool* `FALSE` if set identified by key is empty or doesn't exist.

#### *Example*

```
$vedis->sAdd('key1' , 'member1');
$vedis->sAdd('key1' , 'member2');
$vedis->sAdd('key1' , 'member3'); /* 'key1' => {'member1', 'member2', 'member3'}*/
$vedis->sPeek('key1'); /* member3, 'key1' => {'member1', 'member2'} */
$vedis->sPeek('key1'); /* member2, 'key1' => {'member1'} */
```

----
### sTop

_**Description**_: Get the first element in a set

#### *Parameters*

*key*

#### *Return value*

*String* "popped" value
*Bool* `FALSE` if set identified by key is empty or doesn't exist.

#### *Example*

```
$vedis->sAdd('key1' , 'member1');
$vedis->sAdd('key1' , 'member2');
$vedis->sAdd('key1' , 'member3'); /* 'key1' => {'member1', 'member2', 'member3'}*/
$vedis->sTop('key1'); /* member3, 'key1' => {'member1', 'member2'} */
$vedis->sTop('key1'); /* member2, 'key1' => {'member1'} */
```


## Misc

* [begin](#begin) - Start a write transaction
* [commit](#commit) - Commit an active write transaction
* [rollback](#rollback) - Rollback an active write transaction
* [cmdList](#cmdlist) - List of installed vedis commands
* [eval](#eval) - Execute one or more Vedis commands
* [credits](#credits) - Expand the vedis signature and copyright notice

----
### begin

_**Description**_: Start a write transaction

#### *Return value*

*Bool* `TRUE` if the command is successful.

----
### commit

_**Description**_: Commit an active write transaction

#### *Return value*

*Bool* `TRUE` if the command is successful.

----
### rollback

_**Description**_: Rollback an active write transaction

#### *Return value*

*Bool* `TRUE` if the command is successful.

----
### cmdList

_**Description**_: List of installed vedis commands

#### *Return value*

*Array*: Array of installed vedis commands

----
### eval

_**Description**_: Execute one or more Vedis commands

#### *Parameters*

*STRING*: command

#### *Return value*

Execution result of the command.

#### *Example*

```
$vedis->eval('SET key value; GET key'); /* value */
```

----
### credits

_**Description**_: Expand the vedis signature and copyright notice

#### *Return value*

*String*: Expand the vedis signature and copyright notice


## TODO

* Tests
* Serializer
