--TEST--
Check geohash_verify_hash
--SKIPIF--
<?php if (!extension_loaded("geohash")) print "skip"; ?>
--FILE--
<?php 


$a = array(
        'xp4ge1b',
        'xp4ge1',
        'xp4ge',
        'xp4g',
        'xp4'
        );

foreach($a  as $v) {
    var_export(geohash_verify_hash($v)); echo "\n";
}
?>
--EXPECT--
true
true
true
true
true
