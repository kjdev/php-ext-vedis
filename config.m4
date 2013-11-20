dnl config.m4 for extension vedis

dnl Check PHP version:
AC_MSG_CHECKING(PHP version)
if test ! -z "$phpincludedir"; then
    PHP_VERSION=`grep 'PHP_VERSION ' $phpincludedir/main/php_version.h | sed -e 's/.*"\([[0-9\.]]*\)".*/\1/g' 2>/dev/null`
elif test ! -z "$PHP_CONFIG"; then
    PHP_VERSION=`$PHP_CONFIG --version 2>/dev/null`
fi

if test x"$PHP_VERSION" = "x"; then
    AC_MSG_WARN([none])
else
    PHP_MAJOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/g' 2>/dev/null`
    PHP_MINOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/g' 2>/dev/null`
    PHP_RELEASE_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/g' 2>/dev/null`
    AC_MSG_RESULT([$PHP_VERSION])
fi

if test $PHP_MAJOR_VERSION -lt 5; then
    AC_MSG_ERROR([need at least PHP 5 or newer])
fi

dnl Vedis Extension
PHP_ARG_ENABLE(vedis, whether to enable vedis support,
[  --enable-vedis      Enable vedis support])

if test "$PHP_VEDIS" != "no"; then
    dnl Checks for header files.
dnl    AC_CHECK_HEADERS([locale.h])

    dnl Checks for typedefs, structures, and compiler characteristics.
dnl    AC_TYPE_INT32_T
dnl    AC_TYPE_LONG_LONG_INT

dnl    AC_C_INLINE
dnl    case $ac_cv_c_inline in
dnl        yes) json_inline=inline;;
dnl        no) json_inline=;;
dnl        *) json_inline=$ac_cv_c_inline;;
dnl    esac
dnl    AC_DEFINE_UNQUOTED(PHP_JSON_INLINE,$json_inline, [ ])

    dnl Checks for library functions.
dnl    AC_CHECK_FUNCS([strtoll localeconv])

dnl    case "$ac_cv_type_long_long_int$ac_cv_func_strtoll" in
dnl        yesyes) json_have_long_long=1;;
dnl        *) json_have_long_long=0;;
dnl    esac
dnl    AC_DEFINE_UNQUOTED(PHP_HAVE_JSON_LONG_LONG,$json_have_long_long, [ ])

dnl    case "$ac_cv_header_locale_h$ac_cv_func_localeconv" in
dnl        yesyes) json_have_localeconv=1;;
dnl        *) json_have_localeconv=0;;
dnl    esac
dnl    AC_DEFINE_UNQUOTED(PHP_HAVE_JSON_LOCALECONV,$json_have_localeconv, [ ])

    dnl Source: jansson
dnl    JANSSON_SOURCES="jansson/dump.c jansson/hashtable.c jansson/memory.c jansson/strbuffer.c jansson/utf.c jansson/error.c jansson/load.c jansson/pack_unpack.c jansson/strconv.c jansson/value.c"

    dnl Source: Vedis
    VEDIS_SOURCES="vedis/vedis.c"

    dnl PHP Extension
dnl    PHP_NEW_EXTENSION(vedis, vedis.c vedis_db.c vedis_kvs.c vedis_doc.c vedis_json.c vedis_exception.c $VEDIS_SOURCES $JANSSON_SOURCES, $ext_shared)
    PHP_NEW_EXTENSION(vedis, vedis.c $VEDIS_SOURCES, $ext_shared)

    ifdef([PHP_INSTALL_HEADERS],
    [
        PHP_INSTALL_HEADERS([ext/vedis/], [php_vedis.h])
    ], [
        PHP_ADD_MAKEFILE_FRAGMENT
    ])
fi

dnl coverage
PHP_ARG_ENABLE(coverage, whether to enable coverage support,
[  --enable-coverage     Enable coverage support], no, no)

if test "$PHP_COVERAGE" != "no"; then
    EXTRA_CFLAGS="--coverage"
    PHP_SUBST(EXTRA_CFLAGS)
fi
