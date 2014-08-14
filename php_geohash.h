
#ifndef PHP_GEOHASH_H
#define PHP_GEOHASH_H

extern zend_module_entry geohash_module_entry;
#define phpext_geohash_ptr &geohash_module_entry

#ifdef PHP_WIN32
#	define PHP_GEOHASH_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GEOHASH_API __attribute__ ((visibility("default")))
#else
#	define PHP_GEOHASH_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(geohash);
PHP_MSHUTDOWN_FUNCTION(geohash);
PHP_RINIT_FUNCTION(geohash);
PHP_RSHUTDOWN_FUNCTION(geohash);
PHP_MINFO_FUNCTION(geohash);

PHP_FUNCTION(confirm_geohash_compiled);	/* For testing, remove later. */
PHP_FUNCTION(geohash_verify_hash);
PHP_FUNCTION(geohash_encode);
PHP_FUNCTION(geohash_decode);
PHP_FUNCTION(geohash_get_neighbors);
PHP_FUNCTION(geohash_get_adjacent);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(geohash)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(geohash)
*/

/* In every utility function you add that needs to use variables 
   in php_geohash_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as GEOHASH_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define GEOHASH_G(v) TSRMG(geohash_globals_id, zend_geohash_globals *, v)
#else
#define GEOHASH_G(v) (geohash_globals.v)
#endif

#endif	/* PHP_GEOHASH_H */

