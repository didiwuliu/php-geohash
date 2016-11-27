
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_geohash.h"
#include "geohash_lib.h"

const int MAX_HASH_LENGTH = 22;

/* If you declare any globals in php_geohash.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(geohash)
*/

/* True global resources - no need for thread safety here */
static int le_geohash;

/* {{{ geohash_functions[]
 *
 * Every user visible function must have an entry in geohash_functions[].
 */
const zend_function_entry geohash_functions[] = {
	PHP_FE(confirm_geohash_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(geohash_verify_hash,	NULL)
	PHP_FE(geohash_encode,	NULL)
	PHP_FE(geohash_decode,	NULL)
	PHP_FE(geohash_get_neighbors,	NULL)
	PHP_FE(geohash_get_adjacent,	NULL)
	PHP_FE_END	/* Must be the last line in geohash_functions[] */
};
/* }}} */

/* {{{ geohash_module_entry
 */
zend_module_entry geohash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"geohash",
	geohash_functions,
	PHP_MINIT(geohash),
	PHP_MSHUTDOWN(geohash),
	PHP_RINIT(geohash),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(geohash),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(geohash),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GEOHASH
ZEND_GET_MODULE(geohash)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("geohash.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_geohash_globals, geohash_globals)
    STD_PHP_INI_ENTRY("geohash.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_geohash_globals, geohash_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_geohash_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_geohash_init_globals(zend_geohash_globals *geohash_globals)
{
	geohash_globals->global_value = 0;
	geohash_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(geohash)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(geohash)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(geohash)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(geohash)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(geohash)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "geohash support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_geohash_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_geohash_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "geohash", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto bool geohash_verify_hash(string hash)
   ; */
PHP_FUNCTION(geohash_verify_hash)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(hash_len > 0 && GEOHASH_verify_hash(hash)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string geohash_encode(double latitude, double longitude, long length)
   ; */
PHP_FUNCTION(geohash_encode)
{
	int argc = ZEND_NUM_ARGS();
	long length;
	double latitude;
	double longitude;

	if (zend_parse_parameters(argc TSRMLS_CC, "ddl", &latitude, &longitude, &length) == FAILURE) {
		RETURN_FALSE;
	}
	// check the coordinates
	if(latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
		RETURN_FALSE;
	}
	// check the length
	if(length < 1 || length > MAX_HASH_LENGTH) {
		RETURN_FALSE;
	}

	char * hash = GEOHASH_encode(latitude, longitude, length);
	if(hash == NULL) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(hash, strlen(hash), 0);
}
/* }}} */

/* {{{ proto array geohash_decode(string hash)
   ; */
PHP_FUNCTION(geohash_decode)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(hash_len == 0) {
		RETURN_FALSE;
	}

	GEOHASH_area * area = GEOHASH_decode(hash);
	if(area == NULL) {
		RETURN_FALSE;
	}
	array_init(return_value);

	add_assoc_double(return_value, "lat", (area->latitude.max + area->latitude.min) / 2.0);
	add_assoc_double(return_value, "lng", (area->longitude.max + area->longitude.min) / 2.0);
	add_assoc_double(return_value, "north", area->latitude.max);
	add_assoc_double(return_value, "south", area->latitude.min);
	add_assoc_double(return_value, "east", area->longitude.max);
	add_assoc_double(return_value, "west", area->longitude.min);
	GEOHASH_free_area(area);
}
/* }}} */

/* {{{ proto mixed geohash_get_neighbors(string hash)
   ; */
PHP_FUNCTION(geohash_get_neighbors)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}
	if(hash_len == 0) {
		RETURN_FALSE;
	}

	GEOHASH_neighbors * neighbors = GEOHASH_get_neighbors(hash);
	if(neighbors == NULL) {
		RETURN_FALSE;
	}

#define ADD_ASSOC_STRINGL(return_value, neighbors, name) \
	add_assoc_stringl((return_value), #name, neighbors->name, strlen(neighbors->name), 0)

	array_init(return_value);
	ADD_ASSOC_STRINGL(return_value, neighbors, north);
	ADD_ASSOC_STRINGL(return_value, neighbors, east);
	ADD_ASSOC_STRINGL(return_value, neighbors, west);
	ADD_ASSOC_STRINGL(return_value, neighbors, south);
	ADD_ASSOC_STRINGL(return_value, neighbors, north_east);
	ADD_ASSOC_STRINGL(return_value, neighbors, south_east);
	ADD_ASSOC_STRINGL(return_value, neighbors, north_west);
	ADD_ASSOC_STRINGL(return_value, neighbors, south_west);

	//GEOHASH_free_neighbors(neighbors);
	efree(neighbors);
}
/* }}} */

/* {{{ proto string geohash_get_adjacent(string hash, long direction)
   ; */
PHP_FUNCTION(geohash_get_adjacent)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len;
	long direction;

	if (zend_parse_parameters(argc TSRMLS_CC, "sl", &hash, &hash_len, &direction) == FAILURE) {
		RETURN_FALSE;
	}

	if(direction < GEOHASH_NORTH || direction > GEOHASH_SOUTH) {
		RETURN_FALSE;
	}

	char * _hash = GEOHASH_get_adjacent(hash, direction);
	if(_hash == NULL) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(_hash, strlen(_hash), 0);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
