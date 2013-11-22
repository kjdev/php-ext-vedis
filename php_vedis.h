#ifndef PHP_VEDIS_H
#define PHP_VEDIS_H

#include "vedis/vedis.h"

#define VEDIS_EXT_VERSION "0.1.2"

extern zend_module_entry vedis_module_entry;
#define phpext_vedis_ptr &vedis_module_entry

#ifdef PHP_WIN32
#    define PHP_VEDIS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_VEDIS_API __attribute__ ((visibility("default")))
#else
#    define PHP_VEDIS_API
#endif

#ifdef ZTS
#    include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(vedis)
ZEND_END_MODULE_GLOBALS(vedis)

#ifdef ZTS
#    define VEDIS_G(v) TSRMG(vedis_globals_id, zend_vedis_globals *, v)
#else
#    define VEDIS_G(v) (vedis_globals.v)
#endif

#endif  /* PHP_VEDIS_H */
