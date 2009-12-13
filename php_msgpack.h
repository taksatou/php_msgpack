#include "pack.h"
#include "unpack.h"
#define MSGPACK_PACK_INITBUFSIZE 1024

#ifndef PHP_MSGPACK_H
#define PHP_MSGPACK_H

extern zend_module_entry msgpack_module_entry;
#define phpext_msgpack_ptr &msgpack_module_entry

#ifdef PHP_WIN32
#define PHP_MSGPACK_API __declspec(dllexport)
#else
#define PHP_MSGPACK_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(msgpack);
PHP_MSHUTDOWN_FUNCTION(msgpack);
PHP_RINIT_FUNCTION(msgpack);
PHP_RSHUTDOWN_FUNCTION(msgpack);
PHP_MINFO_FUNCTION(msgpack);

static zend_class_entry *msgpack_ce;
PHP_FUNCTION(msgpack_pack);
PHP_FUNCTION(msgpack_unpack);

PHP_METHOD(msgpack, packer);
PHP_METHOD(msgpack, unpacker);
PHP_METHOD(msgpack, feed);
PHP_METHOD(msgpack, remain);
PHP_METHOD(msgpack, dump);
PHP_METHOD(msgpack, init);
PHP_METHOD(msgpack, setopt);

#define MSGPACK_MAXBUFSIZE 1024*1024
/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     
*/
ZEND_BEGIN_MODULE_GLOBALS(msgpack)
	char unpacker_buf[MSGPACK_MAXBUFSIZE];
ZEND_END_MODULE_GLOBALS(msgpack)


/* In every utility function you add that needs to use variables 
   in php_msgpack_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MSGPACK_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MSGPACK_G(v) TSRMG(msgpack_globals_id, zend_msgpack_globals *, v)
#else
#define MSGPACK_G(v) (msgpack_globals.v)
#endif

#endif	/* PHP_MSGPACK_H */

