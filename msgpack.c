#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_msgpack.h"

ZEND_DECLARE_MODULE_GLOBALS(msgpack)

/* True global resources - no need for thread safety here */
static int le_msgpack;

/* {{{ msgpack_functions[]
 *
 * Every user visible function must have an entry in msgpack_functions[].
 */
zend_function_entry msgpack_functions[] = {
	PHP_FE(msgpack_pack,	NULL)
	PHP_FE(msgpack_unpack,	NULL)
	PHP_ME(msgpack, packer,	NULL, 0)
	PHP_ME(msgpack, unpacker,	NULL, 0)
	PHP_ME(msgpack, feed,	NULL, 0)
	PHP_ME(msgpack, remain,	NULL, 0)
	PHP_ME(msgpack, dump,	NULL, 0)
	PHP_ME(msgpack, init,	NULL, 0)
	PHP_ME(msgpack, setopt,	NULL, 0)
	{NULL, NULL, NULL}	/* Must be the last line in msgpack_functions[] */
};
/* }}} */

/* {{{ msgpack_module_entry
 */
zend_module_entry msgpack_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"msgpack",
	msgpack_functions,
	PHP_MINIT(msgpack),
	PHP_MSHUTDOWN(msgpack),
	PHP_RINIT(msgpack),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(msgpack),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(msgpack),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MSGPACK
ZEND_GET_MODULE(msgpack)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("msgpack.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_msgpack_globals, msgpack_globals)
    STD_PHP_INI_ENTRY("msgpack.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_msgpack_globals, msgpack_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_msgpack_init_globals
static void php_msgpack_init_globals(zend_msgpack_globals *msgpack_globals)
{
	msgpack_globals->maxbuf = 1024*1024;
	msgpack_globals->unpacker_buf = emalloc(sizeof(char) * msgpack_globals->maxbuf);
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
#define MSGPACK_ENABLE_ARRAY 0x1
#define REGISTER_MSGPACK_CONSTANT(__c) REGISTER_LONG_CONSTANT(#__c, __c, CONST_CS | CONST_PERSISTENT)

PHP_MINIT_FUNCTION(msgpack)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Msgpack", msgpack_functions);

	REGISTER_MSGPACK_CONSTANT(MSGPACK_ENABLE_ARRAY);
	msgpack_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_string(msgpack_ce,
							   "buf", strlen("buf"),
							   "", ZEND_ACC_PRIVATE);
	zend_declare_property_long(msgpack_ce,
							   "buf_size", strlen("buf_size"),
							   0, ZEND_ACC_PRIVATE);
	zend_declare_property_long(msgpack_ce,
							   "opts", strlen("opts"),
							   0, ZEND_ACC_PRIVATE);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(msgpack)
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
PHP_RINIT_FUNCTION(msgpack)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(msgpack)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(msgpack)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "msgpack support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ proto string msgpack_pack(mixed obj)
   msgpack packing method */

static void msgpack_pack_zval(msgpack_packer *pk, zval *obj, int enable_array);

PHP_FUNCTION(msgpack_pack)
{
	int argc = ZEND_NUM_ARGS();
	zval *obj = NULL;
	msgpack_packer *pk;

	if (zend_parse_parameters(argc TSRMLS_CC, "z", &obj) == FAILURE) 
		return;

	pk = emalloc(sizeof(msgpack_packer));
	pk->buf = emalloc(MSGPACK_PACK_INITBUFSIZE*(sizeof(size_t)));
   	pk->length = 0;
	pk->buf_size = MSGPACK_PACK_INITBUFSIZE;
	msgpack_pack_zval(pk, obj, 0);
	RETVAL_STRINGL(pk->buf, pk->length, 0);
}

static void msgpack_pack_zval(msgpack_packer *pk, zval *obj, int enable_array)
{	
	switch (Z_TYPE_P(obj)) {
	case IS_NULL:
		msgpack_pack_nil(pk);
		break;
	case IS_LONG:
		msgpack_pack_long(pk, Z_LVAL_P(obj));
		break;
	case IS_DOUBLE:
		msgpack_pack_double(pk, Z_DVAL_P(obj));
		break;
	case IS_BOOL:
		if (Z_BVAL_P(obj))
			msgpack_pack_true(pk);
		else
			msgpack_pack_false(pk);
		break;
	case IS_ARRAY:
		{
			// ref: php-x.x.x/ext/json/json.c 
			HashTable *myht;
			int i;
			char *key;
			zval **data;
			ulong index;
			uint key_len;
			HashPosition pos;
			
            myht = HASH_OF(obj);
			if (enable_array) {
				msgpack_pack_array(pk, zend_hash_num_elements(myht));
			} else {
				msgpack_pack_map(pk, zend_hash_num_elements(myht));
			}
			zend_hash_internal_pointer_reset_ex(myht, &pos);
			for (;; zend_hash_move_forward_ex(myht, &pos)) {
				i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
				if (i == HASH_KEY_NON_EXISTANT)
					break;
				
				if (zend_hash_get_current_data_ex(myht, (void **) &data, &pos) == SUCCESS) {
					if (enable_array) {
						;
					} else {
						if (i == HASH_KEY_IS_STRING) {
							msgpack_pack_raw(pk, key_len);
							msgpack_pack_raw_body(pk, key, key_len);
						} else {
							msgpack_pack_long(pk, index);
						}
					}
					msgpack_pack_zval(pk, *data, enable_array);
				}
			}
		}
		break;
	case IS_STRING:
		msgpack_pack_raw(pk, Z_STRLEN_P(obj));
		msgpack_pack_raw_body(pk, Z_STRVAL_P(obj), Z_STRLEN_P(obj));
		break;
	default:
		break;
	}
}
/* }}} */

/* {{{ proto mixed msgpack_unpack(string obj)
   msgpack unpacking method */
PHP_FUNCTION(msgpack_unpack)
{
	char *obj = NULL;
	int argc = ZEND_NUM_ARGS();
	int obj_len;
	zval *z;

    template_context ctx;
    size_t off = 0;
    int ret;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &obj, &obj_len) == FAILURE) 
		return;

	if (!obj_len) {
		RETURN_NULL();
	}		
	template_init(&ctx);
	ret = template_execute(&ctx, obj, obj_len, &off);
	if (ret == 1) {
		z = template_data(&ctx);
		RETURN_ZVAL(z, 0, 0);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto string Msgpack->pack(mixed obj)
   msgpack packing method */
PHP_METHOD(msgpack, packer)
{
	int argc = ZEND_NUM_ARGS();
	zval *obj = NULL;
	msgpack_packer *pk;
	zval *z, *opts;

	if (zend_parse_parameters(argc TSRMLS_CC, "z", &obj) == FAILURE) 
		return;

	z = getThis();
	opts = zend_read_property(Z_OBJCE_P(z), z, "opts",
							  strlen("opts"), 1 TSRMLS_CC);

	pk = emalloc(sizeof(msgpack_packer));
	pk->buf = emalloc(MSGPACK_PACK_INITBUFSIZE*(sizeof(size_t)));
   	pk->length = 0;
	pk->buf_size = MSGPACK_PACK_INITBUFSIZE;
	msgpack_pack_zval(pk, obj, Z_LVAL_P(opts) & MSGPACK_ENABLE_ARRAY);
	RETVAL_STRINGL(pk->buf, pk->length, 0);
}
/* }}} */


static char * strncat_bin(char *dest, size_t offset, char *src, size_t size)
{
	int i;

	for (i = 0; i < size; i++) {
		*(dest+i+offset) = *src++;
	}
	return dest;
}

/* {{{ proto string Msgpack->unpack([string obj])
   msgpack unpacking method */
PHP_METHOD(msgpack, unpacker)
{	
	int argc = ZEND_NUM_ARGS();
	zval *zval_arg = NULL;
	zval *retval;
    template_context ctx;
    size_t off = 0;
    int ret;
	zval *obj, *buf, *buf_size, *opts;


	if (zend_parse_parameters(argc TSRMLS_CC, "z", &zval_arg) == FAILURE) 
		return;

	obj = getThis();
	buf = zend_read_property(Z_OBJCE_P(obj), obj, "buf",
							 strlen("buf"), 1 TSRMLS_CC);
	buf_size = zend_read_property(Z_OBJCE_P(obj), obj, "buf_size",
								  strlen("buf_size"), 1 TSRMLS_CC);
	opts = zend_read_property(Z_OBJCE_P(obj), obj, "opts",
							  strlen("opts"), 1 TSRMLS_CC);

	template_init(&ctx);
	ctx.user.enable_array = Z_LVAL_P(opts) & MSGPACK_ENABLE_ARRAY;
	if (Z_TYPE_P(zval_arg) == IS_NULL) {
		char *buf_p;
		long next_len;
	
		next_len = Z_LVAL_P(buf_size);
		ret = template_execute(&ctx, Z_STRVAL_P(buf), next_len, &off);
		if (ret == 1) {
			next_len -= off;
			buf_p = MSGPACK_G(unpacker_buf);
			strncat_bin(buf_p, 0, Z_STRVAL_P(buf) + off, next_len);
			zend_update_property_long(Z_OBJCE_P(obj), obj, "buf_size", strlen("buf_size"), next_len TSRMLS_CC);
			zend_update_property_stringl(Z_OBJCE_P(obj), obj, "buf", strlen("buf"), buf_p, next_len TSRMLS_CC);
			retval = template_data(&ctx);
			RETURN_ZVAL(retval, 0, 0);
		} else {
			RETURN_NULL();
		}
	} else if (Z_TYPE_P(zval_arg) == IS_STRING) {
		/* same as msgpack_unpack */
		ret = template_execute(&ctx, Z_STRVAL_P(zval_arg), Z_STRLEN_P(zval_arg), &off);
		if (ret == 1) {
			retval = template_data(&ctx);
			RETURN_ZVAL(retval, 0, 0);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto string Msgpack->pack(mixed obj)
   msgpack packing method */
PHP_METHOD(msgpack, feed)
{
	zval *buf, *buf_size, *obj;
	char *str = NULL;
	int argc = ZEND_NUM_ARGS();
	int str_len;
	zval *z;
	long next_len;
	char *buf_p;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &str, &str_len) == FAILURE) 
		return;
	if (!str_len) {
		RETURN_TRUE;
	}

	obj = getThis();
	buf = zend_read_property(Z_OBJCE_P(obj), obj, "buf",
							 strlen("buf"), 1 TSRMLS_CC);
	buf_size = zend_read_property(Z_OBJCE_P(obj), obj, "buf_size",
							 strlen("buf_size"), 1 TSRMLS_CC);
	next_len  = str_len + Z_LVAL_P(buf_size);
	if (next_len > MSGPACK_MAXBUFSIZE) {
		printf("error: Too many feeds. Unpacker buffer size is %d\n", MSGPACK_MAXBUFSIZE);
		RETURN_FALSE;
	}
	buf_p = MSGPACK_G(unpacker_buf);
	strncat_bin(buf_p, 0, Z_STRVAL_P(buf), Z_STRLEN_P(buf));
	strncat_bin(buf_p, Z_STRLEN_P(buf), str, str_len);
	zend_update_property_long(Z_OBJCE_P(obj), obj, "buf_size", strlen("buf_size"), next_len TSRMLS_CC);
	zend_update_property_stringl(Z_OBJCE_P(obj), obj, "buf", strlen("buf"), buf_p, next_len TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string Msgpack->pack(mixed obj)
   msgpack packing method */
PHP_METHOD(msgpack, remain)
{
	zval *data, *obj;

	obj = getThis();
	data = zend_read_property(Z_OBJCE_P(obj), obj, "buf_size",
							  strlen("buf_size"), 1 TSRMLS_CC);
	RETVAL_LONG(Z_LVAL_P(data));
}
/* }}} */


/* this is read-only, so it's ok */
static char hexconvtab[] = "0123456789abcdef";

/* {{{ php_bin2hex
 */
static char *php_bin2hex(const unsigned char *old, const size_t oldlen, size_t *newlen)
{
    register unsigned char *result = NULL;
    size_t i, j;

    result = (char *) safe_emalloc(oldlen * 2, sizeof(char), 1);

    for (i = j = 0; i < oldlen; i++) {
        result[j++] = hexconvtab[old[i] >> 4];
        result[j++] = hexconvtab[old[i] & 15];
    }
    result[j] = '\0';

    if (newlen)
        *newlen = oldlen * 2 * sizeof(char);

    return result;
}
/* }}} */

/* {{{ proto string Msgpack->dump()
   msgpack dump */
PHP_METHOD(msgpack, dump)
{
	zval *data, *obj;
	char *result;
	size_t newlen;

	obj = getThis();
	data = zend_read_property(Z_OBJCE_P(obj), obj, "buf",
							  strlen("buf"), 1 TSRMLS_CC);
    result = php_bin2hex(Z_STRVAL_P(data), Z_STRLEN_P(data), &newlen);

    if (!result) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(result, newlen, 0);

}
/* }}} */

/* {{{ proto string Msgpack->init()
   msgpack init */
PHP_METHOD(msgpack, init)
{
	zval *buf, *buf_size, *obj;

	obj = getThis();
	buf = zend_read_property(Z_OBJCE_P(obj), obj, "buf",
							 strlen("buf"), 1 TSRMLS_CC);
	buf_size = zend_read_property(Z_OBJCE_P(obj), obj, "buf_size",
							 strlen("buf_size"), 1 TSRMLS_CC);
	zend_update_property_long(Z_OBJCE_P(obj), obj, "buf_size", strlen("buf_size"), 0 TSRMLS_CC);
	zend_update_property_stringl(Z_OBJCE_P(obj), obj, "buf", strlen("buf"), "", 0 TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string Msgpack->setopt()
   msgpack set option */
PHP_METHOD(msgpack, setopt)
{
	int argc = ZEND_NUM_ARGS();
	long optid, o;
	zend_bool b;
	zval *obj, *opts;

	if (zend_parse_parameters(argc TSRMLS_CC, "lb", &optid, &b) == FAILURE)
		return;

	obj = getThis();
	opts = zend_read_property(Z_OBJCE_P(obj), obj, "opts",
							 strlen("opts"), 1 TSRMLS_CC);
	o = Z_LVAL_P(opts);
	switch (optid) {
	case MSGPACK_ENABLE_ARRAY:
		ZVAL_LONG(opts, b ? o | 0x1 : o & ~0x1);
		break;
	default:
		break;
	}
	RETURN_TRUE;
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
