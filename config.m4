dnl $Id$
dnl config.m4 for extension wordcheck

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(wordcheck, for wordcheck support,
Make sure that the comment is aligned:
[  --with-wordcheck             Include wordcheck support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(wordcheck, whether to enable wordcheck support,
dnl Make sure that the comment is aligned:
dnl [  --enable-wordcheck           Enable wordcheck support])

if test "$PHP_WORDCHECK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-wordcheck -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/wordcheck.h"  # you most likely want to change this
  dnl if test -r $PHP_WORDCHECK/$SEARCH_FOR; then # path given as parameter
  dnl   WORDCHECK_DIR=$PHP_WORDCHECK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for wordcheck files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       WORDCHECK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$WORDCHECK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the wordcheck distribution])
  dnl fi

  dnl # --with-wordcheck -> add include path
  dnl PHP_ADD_INCLUDE($WORDCHECK_DIR/include)

  dnl # --with-wordcheck -> check for lib and symbol presence
  dnl LIBNAME=wordcheck # you may want to change this
  dnl LIBSYMBOL=wordcheck # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WORDCHECK_DIR/lib, WORDCHECK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_WORDCHECKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong wordcheck lib version or lib not found])
  dnl ],[
  dnl   -L$WORDCHECK_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(WORDCHECK_SHARED_LIBADD)
	
  CFLAGS="-I/usr/local/wordcheck/include `pkg-config --cflags --libs glib-2.0`"
  LDFLAGS="-L/usr/local/wordcheck/lib/wordcheck -lwordcheck"

  PHP_NEW_EXTENSION(wordcheck, wordcheck.c, $ext_shared)
fi
