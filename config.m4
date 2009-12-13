dnl $Id$
dnl config.m4 for extension msgpack

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(msgpack, for msgpack support,
dnl Make sure that the comment is aligned:
dnl [  --with-msgpack             Include msgpack support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(msgpack, whether to enable msgpack support,
Make sure that the comment is aligned:
[  --enable-msgpack           Enable msgpack support])

if test "$PHP_MSGPACK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-msgpack -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/msgpack.h"  # you most likely want to change this
  dnl if test -r $PHP_MSGPACK/$SEARCH_FOR; then # path given as parameter
  dnl   MSGPACK_DIR=$PHP_MSGPACK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for msgpack files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MSGPACK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MSGPACK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the msgpack distribution])
  dnl fi

  dnl # --with-msgpack -> add include path
  dnl PHP_ADD_INCLUDE($MSGPACK_DIR/include)

  dnl # --with-msgpack -> check for lib and symbol presence
  dnl LIBNAME=msgpack # you may want to change this
  dnl LIBSYMBOL=msgpack # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MSGPACK_DIR/lib, MSGPACK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MSGPACKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong msgpack lib version or lib not found])
  dnl ],[
  dnl   -L$MSGPACK_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(MSGPACK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(msgpack, msgpack.c, $ext_shared)
fi
