#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_vedis.h"

ZEND_DECLARE_MODULE_GLOBALS(vedis)

ZEND_INI_BEGIN()
ZEND_INI_END()

zend_class_entry *php_vedis_ce;
static zend_object_handlers php_vedis_handlers;

static int le_vedis;

typedef struct {
    vedis *store;
    char *storage;
    zend_bool is_persistent;
    int pid;
} php_vedis_t;

typedef struct {
    zend_object std;
    php_vedis_t *vedis;
} php_vedis_object_t;

typedef struct {
    vedis_value **data;
    size_t size;
    size_t used;
    int rc;
    int free;
} php_vedis_args;

typedef SyString vedis_string;

ZEND_BEGIN_ARG_INFO_EX(arginfo_vedis___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, storage)
    ZEND_ARG_INFO(0, persistent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_vedis_none, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_vedis_param1, 0, 0, 1)
    ZEND_ARG_INFO(0, param1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_vedis_param2, 0, 0, 2)
    ZEND_ARG_INFO(0, param1)
    ZEND_ARG_INFO(0, param2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_vedis_param3, 0, 0, 3)
    ZEND_ARG_INFO(0, param1)
    ZEND_ARG_INFO(0, param2)
    ZEND_ARG_INFO(0, param3)
ZEND_END_ARG_INFO()

#define VEDIS_SELF(self)                                       \
    self = (php_vedis_object_t *)zend_object_store_get_object( \
        getThis() TSRMLS_CC)

#define VEDIS_EXCEPTION(...)                                \
    zend_throw_exception_ex(NULL, 0 TSRMLS_CC, __VA_ARGS__)

#define VEDIS_PARAM(_command, _length)          \
    int rc;                                     \
    php_vedis_object_t *intern;                 \
    php_vedis_args args;                        \
    vedis_string cmd = { # _command, _length }; \
    vedis_value *result = NULL

#define VEDIS_ARGS_PARAM(_command, _length, _argc) \
    vedis_value *argv[_argc] = { 0, };             \
    VEDIS_PARAM(_command, _length);                \
    VEDIS_ARGS_INIT_DATA(argv, _argc)

#define VEDIS_ARGS_INIT_DATA(_data, _size) \
    args.data = _data;                     \
    args.size = _size;                     \
    args.free = 0;                         \
    args.used = 0;                         \
    args.rc = 0

#define VEDIS_ARGS_EXEC(_return)                                            \
    if (!php_vedis_args_success(&args)) {                                   \
        php_vedis_args_destroy(&args, 1);                                   \
        _return;                                                            \
    }                                                                       \
    rc = vedis_exec_args(intern->vedis->store, &cmd, args.size, args.data); \
    php_vedis_args_destroy(&args, 0);                                       \
    if (rc != VEDIS_OK) {                                                   \
        php_vedis_error(intern, E_WARNING TSRMLS_CC);                       \
        _return;                                                            \
    }                                                                       \
    vedis_exec_result(intern->vedis->store, &result);                       \
    if (!result) {                                                          \
        php_vedis_error(intern, E_WARNING TSRMLS_CC);                       \
        _return;                                                            \
    }

#define VEDIS_ARGS_INIT(_size)                          \
    if (php_vedis_args_init(&args, _size) != SUCCESS) { \
        RETURN_FALSE;                                   \
    }

#define VEDIS_ARGS_STRING(_str, _len)                    \
    php_vedis_args_set_string(&args, intern, _str, _len)

#define VEDIS_ARGS_INT(_num)                    \
    php_vedis_args_set_int(&args, intern, _num)

#define VEDIS_RETURN_BOOL()             \
    if (!vedis_value_to_bool(result)) { \
        RETURN_FALSE;                   \
    }                                   \
    RETURN_TRUE

#define VEDIS_RETURN_LONG()                   \
    RETURN_LONG(vedis_value_to_int64(result))

#define VEDIS_RETURN_STRING()                          \
    if (vedis_value_is_null(result)) {                 \
        RETURN_NULL();                                 \
    }                                                  \
    RETURN_STRING(vedis_value_to_string(result, 0), 1)

#define VEDIS_RETURN_ARRAY()                                               \
    array_init(return_value);                                              \
    if (vedis_value_is_array(result)) {                                    \
        vedis_value *entry;                                                \
        while ((entry = vedis_array_next_elem(result)) != 0) {             \
            if (vedis_value_is_null(entry)) {                              \
                add_next_index_null(return_value);                         \
            } else {                                                       \
                add_next_index_string(return_value,                        \
                                      vedis_value_to_string(entry, 0), 1); \
            }                                                              \
        }                                                                  \
    }


static void php_vedis_store_destroy(php_vedis_t *vedis)
{
    vedis_close(vedis->store);
    pefree(vedis, vedis->is_persistent);
}

static int php_vedis_store_list_entry(void)
{
    return le_vedis;
}

static php_vedis_t *
php_vedis_new(char *storage, int storage_len, zend_bool is_persistent TSRMLS_DC)
{
    php_vedis_t *vedis;
    char *filepath = NULL, *stragepath = NULL;

    vedis = pecalloc(1, sizeof(php_vedis_t), is_persistent);

    if (!storage || storage_len <= 0) {
        stragepath = NULL;
    } else if (strcmp(storage, ":mem:") != 0) {
        if (!(filepath = expand_filepath(storage, NULL TSRMLS_CC))) {
            pefree(vedis, is_persistent);
            return NULL;
        }
        stragepath = filepath;
    } else {
        stragepath = storage;
    }

    if (vedis_open(&(vedis->store), stragepath) != VEDIS_OK) {
        if (filepath) {
            efree(filepath);
        }
        pefree(vedis, is_persistent);
        return NULL;
    }

    if (filepath) {
        efree(filepath);
    }

    vedis->storage = storage;
    vedis->is_persistent = is_persistent;
    vedis->pid = getpid();

    return vedis;
}

static php_vedis_t *
php_vedis_get(char *storage, int storage_len, zend_bool is_persistent TSRMLS_DC)
{
    php_vedis_t *vedis;
    char plist_key[48];
    int plist_key_len;
    zend_rsrc_list_entry le, *le_p = NULL;

    if (is_persistent) {
        plist_key_len = snprintf(plist_key, 48, "vedis");
        plist_key_len += 1;

        if (zend_hash_find(&EG(persistent_list), plist_key, plist_key_len,
                           (void *)&le_p) == SUCCESS) {
            if (le_p->type == le_vedis) {
                return (php_vedis_t *) le_p->ptr;
            }
        }
    }

    vedis = php_vedis_new(storage, storage_len, is_persistent TSRMLS_CC);
    if (!vedis) {
        return NULL;
    }

    if (is_persistent) {
        le.type = le_vedis;
        le.ptr  = vedis;
        if (zend_hash_update(&EG(persistent_list),
                             (char *)plist_key, plist_key_len, (void *)&le,
                             sizeof(le), NULL) == FAILURE) {
            php_error_docref(NULL TSRMLS_CC, E_ERROR,
                             "Could not register persistent entry for vedis");
        }
    }
    return vedis;
}

static void
php_vedis_error(php_vedis_object_t *intern, int flags TSRMLS_DC)
{
    const char *msg = NULL;
    int len = 0;

    if (intern->vedis && intern->vedis->store) {
        vedis_config(intern->vedis->store, VEDIS_CONFIG_ERR_LOG, &msg, &len);
    }

    if (msg && len > 0) {
        php_error_docref(NULL TSRMLS_CC, flags, msg);
    } else {
        php_error_docref(NULL TSRMLS_CC, flags, "unknown error");
    }
}

static int
php_vedis_args_init(php_vedis_args *args, size_t argc)
{
    args->data = (vedis_value **)emalloc(sizeof(vedis_value *) * argc);
    if (!args->data) {
        return FAILURE;
    }

    memset(args->data, 0, sizeof(vedis_value *) * argc);
    args->size = argc;
    args->used = 0;
    args->rc = 0;
    args->free = 1;

    return SUCCESS;
}

static void
php_vedis_args_destroy(php_vedis_args *args, int obj)
{
    if (args) {
        if (args->data) {
            if (obj) {
                size_t i;
                for (i = 0; i < args->size; i++) {
                    vedis_value_release(args->data[i]);
                }
            }
            if (args->free) {
                efree(args->data);
            }
        }
    }
}

static void
php_vedis_args_set_string(php_vedis_args *args,
                          php_vedis_object_t *intern, char *str, size_t len)
{
    int rc;

    if (args->used >= args->size) {
        args->rc = -1;
        return;
    }

    rc = vedis_value_string_new(intern->vedis->store, str, len,
                                &(args->data[args->used]));
    if (rc == VEDIS_OK) {
        args->used++;
    } else {
        php_vedis_error(intern, E_WARNING TSRMLS_CC);
        args->rc = rc;
    }
}

static void
php_vedis_args_set_int(php_vedis_args *args,
                       php_vedis_object_t *intern, long num)
{
    int rc;

    if (args->used >= args->size) {
        args->rc = -1;
        return;
    }

    rc = vedis_value_int_new(intern->vedis->store, num,
                             &(args->data[args->used]));
    if (rc == VEDIS_OK) {
        args->used++;
    } else {
        php_vedis_error(intern, E_WARNING TSRMLS_CC);
        args->rc = rc;
    }
}

static int
php_vedis_args_success(php_vedis_args *args)
{
    if (args->rc == 0) {
        return 1;
    }
    return 0;
}


ZEND_METHOD(Vedis, __construct)
{
    int rc;
    char *storage = NULL;
    int storage_len = 0;
    zend_bool is_persistent = 1;
    php_vedis_object_t *intern;
#if ZEND_MODULE_API_NO >= 20090626
    zend_error_handling error_handling;
    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
#else
    php_set_error_handling(EH_THROW, NULL TSRMLS_CC);
#endif

    rc = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sb",
                               &storage, &storage_len, &is_persistent);

#if ZEND_MODULE_API_NO >= 20090626
    zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
    php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
    if (rc == FAILURE) {
        VEDIS_EXCEPTION("Error creating: %s", storage);
        return;
    }

    VEDIS_SELF(intern);

    intern->vedis = php_vedis_get(storage, storage_len, is_persistent TSRMLS_CC);
    if (!intern->vedis) {
        VEDIS_EXCEPTION("Error creating: %s", storage);
        return;
    }
}

ZEND_METHOD(Vedis, get)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(GET, 3, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_NULL());

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, set)
{
    char *key, *value;
    int key_len, value_len;
    VEDIS_ARGS_PARAM(SET, 3, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &value, &value_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(value, value_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, setnx)
{
    char *key, *value;
    int key_len, value_len;
    VEDIS_ARGS_PARAM(SETNX, 5, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &value, &value_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(value, value_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, del)
{
    int i, argc = ZEND_NUM_ARGS();
    zval ***params;
    VEDIS_PARAM(DEL, 3);

    if (argc <= 0) {
        WRONG_PARAM_COUNT;
        return;
    }

    params = (zval ***)emalloc(sizeof(zval *) * argc);
    if (!params) {
        WRONG_PARAM_COUNT;
        return;
    }

    if (zend_get_parameters_array_ex(argc, params) != SUCCESS) {
        WRONG_PARAM_COUNT;
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT(argc);

    for (i = 0; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) != IS_STRING) {
            convert_to_string(*params[i]);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i]));
    }

    VEDIS_ARGS_EXEC(RETURN_LONG(0));

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, exists)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(EXISTS, 6, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, incr)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(INCR, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, incrby)
{
    char *key;
    int key_len;
    long num;
    VEDIS_ARGS_PARAM(INCRBY, 6, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl",
                              &key, &key_len, &num) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_INT(num);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, decr)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(DECR, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, decrby)
{
    char *key;
    int key_len;
    long num;
    VEDIS_ARGS_PARAM(DECRBY, 6, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl",
                              &key, &key_len, &num) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_INT(num);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, mget)
{
    zval *keys;
    zval **val;
    size_t n;
    HashPosition pos;
    VEDIS_PARAM(MGET, 4);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a",
                              &keys) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    n = zend_hash_num_elements(HASH_OF(keys));
    if (n == 0) {
        RETURN_FALSE;
    }

    VEDIS_ARGS_INIT(n);

    zend_hash_internal_pointer_reset_ex(HASH_OF(keys), &pos);
    while (zend_hash_get_current_data_ex(HASH_OF(keys),
                                         (void **)&val, &pos) == SUCCESS) {
        if (Z_TYPE_PP(val) != IS_STRING) {
            convert_to_string(*val);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
        zend_hash_move_forward_ex(HASH_OF(keys), &pos);
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, getset)
{
    char *key, *value;
    int key_len, value_len;
    VEDIS_ARGS_PARAM(GETSET, 6, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &value, &value_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(value, value_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, rename)
{
    char *src, *dst;
    int src_len, dst_len;
    VEDIS_ARGS_PARAM(MOVE, 4, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &src, &src_len, &dst, &dst_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(src, src_len);
    VEDIS_ARGS_STRING(dst, dst_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, append)
{
    char *key, *value;
    int key_len, value_len;
    VEDIS_ARGS_PARAM(APPEND, 6, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &value, &value_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(value, value_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, strlen)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(STRLEN, 6, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_LONG(0));

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, mset)
{
    zval **val, *members;
    int key_len;
    HashPosition pos;
    size_t n;
    VEDIS_PARAM(MSET, 4);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a",
                              &members) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    n = zend_hash_num_elements(HASH_OF(members));
    if (n == 0) {
        RETURN_FALSE;
    }

    VEDIS_ARGS_INIT(n * 2);

    zend_hash_internal_pointer_reset_ex(HASH_OF(members), &pos);
    while (zend_hash_get_current_data_ex(HASH_OF(members),
                                         (void **)&val, &pos) == SUCCESS) {
        char *str_key;
        uint str_key_len;
        ulong num_key;
        int flags;

        flags = zend_hash_get_current_key_ex(HASH_OF(members),
                                             &str_key, &str_key_len,
                                             &num_key, 0, &pos);
        if (flags == HASH_KEY_NON_EXISTANT) {
            break;
        }

        if (Z_TYPE_PP(val) != IS_STRING) {
            convert_to_string(*val);
        }

        if (flags == HASH_KEY_IS_STRING) {
            VEDIS_ARGS_STRING(str_key, str_key_len - 1);
        } else {
            VEDIS_ARGS_INT(num_key);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(val), Z_STRLEN_PP(val));

        zend_hash_move_forward_ex(HASH_OF(members), &pos);
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, msetnx)
{
    zval **val, *members;
    int key_len;
    HashPosition pos;
    size_t n;
    VEDIS_PARAM(MSETNX, 6);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a",
                              &members) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    n = zend_hash_num_elements(HASH_OF(members));
    if (n == 0) {
        RETURN_FALSE;
    }

    VEDIS_ARGS_INIT(n * 2);

    zend_hash_internal_pointer_reset_ex(HASH_OF(members), &pos);
    while (zend_hash_get_current_data_ex(HASH_OF(members),
                                         (void **)&val, &pos) == SUCCESS) {
        char *str_key;
        uint str_key_len;
        ulong num_key;
        int flags;

        flags = zend_hash_get_current_key_ex(HASH_OF(members),
                                             &str_key, &str_key_len,
                                             &num_key, 0, &pos);
        if (flags == HASH_KEY_NON_EXISTANT) {
            break;
        }

        if (Z_TYPE_PP(val) != IS_STRING) {
            convert_to_string(*val);
        }

        if (flags == HASH_KEY_IS_STRING) {
            VEDIS_ARGS_STRING(str_key, str_key_len - 1);
        } else {
            VEDIS_ARGS_INT(num_key);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(val), Z_STRLEN_PP(val));

        zend_hash_move_forward_ex(HASH_OF(members), &pos);
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, hset)
{
    char *key, *field, *value;
    int key_len, field_len, value_len;
    VEDIS_ARGS_PARAM(HSET, 4, 3);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                              &key, &key_len, &field, &field_len,
                              &value, &value_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(field, field_len);
    VEDIS_ARGS_STRING(value, value_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE)

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, hsetnx)
{
    char *key, *field, *value;
    int key_len, field_len, value_len;
    VEDIS_ARGS_PARAM(HSETNX, 6, 3);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
                              &key, &key_len, &field, &field_len,
                              &value, &value_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(field, field_len);
    VEDIS_ARGS_STRING(value, value_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, hget)
{
    char *key, *field;
    int key_len, field_len;
    VEDIS_ARGS_PARAM(HGET, 4, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &field, &field_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(field, field_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, hlen)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(HLEN, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_LONG(0));

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, hdel)
{
    char *key, *field;
    int key_len, field_len;
    VEDIS_ARGS_PARAM(HDEL, 4, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &field, &field_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(field, field_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, hkeys)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(HKEYS, 5, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, hvals)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(HVALS, 5, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, hgetall)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(HGETALL, 7, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    array_init(return_value);

    if (vedis_value_is_array(result)) {
        const char *key;
        vedis_value *entry;
        while ((entry = vedis_array_next_elem(result)) != 0) {
            key = vedis_value_to_string(entry, 0);
            if ((entry = vedis_array_next_elem(result)) == 0) {
                break;
            }
            if (vedis_value_is_null(entry)) {
                add_assoc_null_ex(return_value, key, strlen(key) + 1);
            } else {
                add_assoc_string_ex(return_value, key, strlen(key) + 1,
                                    (char *)vedis_value_to_string(entry, 0), 1);
            }
        }
    }
}

ZEND_METHOD(Vedis, hexists)
{
    char *key, *field;
    int key_len, field_len;
    VEDIS_ARGS_PARAM(HEXISTS, 7, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &field, &field_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(field, field_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, hmset)
{
    char *key;
    int key_len;
    zval **val, *members;
    HashPosition pos;
    size_t n;
    VEDIS_PARAM(HMSET, 5);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa",
                              &key, &key_len, &members) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    n = zend_hash_num_elements(HASH_OF(members));
    if (n == 0) {
        RETURN_FALSE;
    }

    VEDIS_ARGS_INIT((n * 2) + 1);

    VEDIS_ARGS_STRING(key, key_len);

    zend_hash_internal_pointer_reset_ex(HASH_OF(members), &pos);
    while (zend_hash_get_current_data_ex(HASH_OF(members),
                                         (void **)&val, &pos) == SUCCESS) {
        char *str_key;
        uint str_key_len;
        ulong num_key;
        int flags;

        flags = zend_hash_get_current_key_ex(HASH_OF(members),
                                             &str_key, &str_key_len,
                                             &num_key, 0, &pos);
        if (flags == HASH_KEY_NON_EXISTANT) {
            break;
        }

        if (Z_TYPE_PP(val) != IS_STRING) {
            convert_to_string(*val);
        }

        if (flags == HASH_KEY_IS_STRING) {
            VEDIS_ARGS_STRING(str_key, str_key_len - 1);
        } else {
            VEDIS_ARGS_INT(num_key);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(val), Z_STRLEN_PP(val));

        zend_hash_move_forward_ex(HASH_OF(members), &pos);
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, hmget)
{
    char *key;
    int key_len;
    zval **val, *fields;
    HashPosition pos;
    size_t n;
    VEDIS_PARAM(HMGET, 5);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa",
                              &key, &key_len, &fields) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    n = zend_hash_num_elements(HASH_OF(fields));
    if (n == 0) {
        RETURN_FALSE;
    }

    VEDIS_ARGS_INIT(n + 1);

    VEDIS_ARGS_STRING(key, key_len);

    zend_hash_internal_pointer_reset_ex(HASH_OF(fields), &pos);
    while (zend_hash_get_current_data_ex(HASH_OF(fields),
                                         (void **)&val, &pos) == SUCCESS) {
        if (Z_TYPE_PP(val) != IS_STRING) {
            convert_to_string(*val);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
        zend_hash_move_forward_ex(HASH_OF(fields), &pos);
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    array_init(return_value);

    if (vedis_value_is_array(result)) {
        vedis_value *entry;
        zend_hash_internal_pointer_reset_ex(HASH_OF(fields), &pos);
        while (1) {
            if (zend_hash_get_current_data_ex(HASH_OF(fields),
                                              (void **)&val, &pos) != SUCCESS) {
                break;
            }
            if ((entry = vedis_array_next_elem(result)) == 0) {
                break;
            }

            if (Z_TYPE_PP(val) != IS_STRING) {
                convert_to_string(*val);
            }

            if (vedis_value_is_null(entry)) {
                add_assoc_null_ex(return_value,
                                  Z_STRVAL_PP(val), Z_STRLEN_PP(val) + 1);
            } else {
                add_assoc_string_ex(return_value,
                                    Z_STRVAL_PP(val), Z_STRLEN_PP(val) + 1,
                                    (char *)vedis_value_to_string(entry, 0), 1);
            }

            zend_hash_move_forward_ex(HASH_OF(fields), &pos);
        }
    }
}

ZEND_METHOD(Vedis, lindex)
{
    char *key;
    int key_len;
    long index;
    VEDIS_ARGS_PARAM(LINDEX, 6, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl",
                              &key, &key_len, &index) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_INT(index);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, lpop)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(LPOP, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, lpush)
{
    int i, argc = ZEND_NUM_ARGS();
    zval ***params;
    VEDIS_PARAM(LPUSH, 5);

    if (argc < 2) {
        WRONG_PARAM_COUNT;
        return;
    }

    params = (zval ***)emalloc(sizeof(zval *) * argc);
    if (!params) {
        WRONG_PARAM_COUNT;
        return;
    }

    if (zend_get_parameters_array_ex(argc, params) != SUCCESS) {
        WRONG_PARAM_COUNT;
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT(argc);

    for (i = 0; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) != IS_STRING) {
            convert_to_string(*params[i]);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i]));
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, llen)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(LLEN, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_LONG(0));

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, sadd)
{
    int i, argc = ZEND_NUM_ARGS();
    zval ***params;
    VEDIS_PARAM(SADD, 4);

    if (argc < 2) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    params = (zval ***)emalloc(sizeof(zval *) * argc);
    if (!params) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    if (zend_get_parameters_array_ex(argc, params) != SUCCESS) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT(argc);

    for (i = 0; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) != IS_STRING) {
            convert_to_string(*params[i]);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i]));
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, scard)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(SCARD, 5, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_LONG(0));

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, sdiff)
{
    int i, argc = ZEND_NUM_ARGS();
    zval ***params;
    VEDIS_PARAM(SDIFF, 5);

    if (argc < 1) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    params = (zval ***)emalloc(sizeof(zval *) * argc);
    if (!params) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    if (zend_get_parameters_array_ex(argc, params) != SUCCESS) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT(argc);

    for (i = 0; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) != IS_STRING) {
            convert_to_string(*params[i]);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i]));
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, sinter)
{
    int i, argc = ZEND_NUM_ARGS();
    zval ***params;
    VEDIS_PARAM(SINTER, 6);

    if (argc < 1) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    params = (zval ***)emalloc(sizeof(zval *) * argc);
    if (!params) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    if (zend_get_parameters_array_ex(argc, params) != SUCCESS) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT(argc);

    for (i = 0; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) != IS_STRING) {
            convert_to_string(*params[i]);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i]));
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, sismember)
{
    char *key, *member;
    int key_len, member_len;
    VEDIS_ARGS_PARAM(SISMEMBER, 9, 2);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
                              &key, &key_len, &member, &member_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);
    VEDIS_ARGS_STRING(member, member_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, smembers)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(SMEMBERS, 8, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, spop)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(SPOP, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_NULL());

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, srem)
{
    int i, argc = ZEND_NUM_ARGS();
    zval ***params;
    VEDIS_PARAM(SREM, 4);

    if (argc < 2) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    params = (zval ***)emalloc(sizeof(zval *) * argc);
    if (!params) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    if (zend_get_parameters_array_ex(argc, params) != SUCCESS) {
        WRONG_PARAM_COUNT;
        RETURN_FALSE;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT(argc);

    for (i = 0; i < argc; i++) {
        if (Z_TYPE_PP(params[i]) != IS_STRING) {
            convert_to_string(*params[i]);
        }
        VEDIS_ARGS_STRING(Z_STRVAL_PP(params[i]), Z_STRLEN_PP(params[i]));
    }

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_LONG();
}

ZEND_METHOD(Vedis, speek)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(SPEEK, 5, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_NULL());

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, stop)
{
    char *key;
    int key_len;
    VEDIS_ARGS_PARAM(STOP, 4, 1);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &key, &key_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_STRING(key, key_len);

    VEDIS_ARGS_EXEC(RETURN_NULL());

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, commit)
{
    char *key;
    int key_len;
    VEDIS_PARAM(COMMIT, 6);

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT_DATA(NULL, 0);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, rollback)
{
    char *key;
    int key_len;
    VEDIS_PARAM(ROLLBACK, 8);

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT_DATA(NULL, 0);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, begin)
{
    char *key;
    int key_len;
    VEDIS_PARAM(BEGIN, 5);

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT_DATA(NULL, 0);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_BOOL();
}

ZEND_METHOD(Vedis, cmdlist)
{
    char *key;
    int key_len;
    VEDIS_PARAM(CMD_LIST, 8);

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT_DATA(NULL, 0);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, tablelist)
{
    char *key;
    int key_len;
    VEDIS_PARAM(TABLE_LIST, 10);

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT_DATA(NULL, 0);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_ARRAY();
}

ZEND_METHOD(Vedis, credits)
{
    char *key;
    int key_len;
    VEDIS_PARAM(VEDIS, 5);

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    VEDIS_ARGS_INIT_DATA(NULL, 0);

    VEDIS_ARGS_EXEC(RETURN_FALSE);

    VEDIS_RETURN_STRING();
}

ZEND_METHOD(Vedis, eval)
{
    char *cmd;
    int cmd_len;
    int rc;
    php_vedis_object_t *intern;
    vedis_value *result = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &cmd, &cmd_len) == FAILURE) {
        return;
    }

    VEDIS_SELF(intern);

    if (vedis_exec(intern->vedis->store, cmd, cmd_len) != VEDIS_OK) {
        php_vedis_error(intern, E_WARNING TSRMLS_CC);
        RETURN_FALSE;
    }

    vedis_exec_result(intern->vedis->store, &result);
    if (!result) {
        php_vedis_error(intern, E_WARNING TSRMLS_CC);
        RETURN_FALSE;
    }

    if (vedis_value_is_array(result)) {
        vedis_value *entry;
        array_init(return_value);
        while ((entry = vedis_array_next_elem(result)) != 0) {
            if (vedis_value_is_null(entry)) {
                add_next_index_null(return_value);
            } else {
                add_next_index_string(return_value,
                                      vedis_value_to_string(entry, 0), 1);
            }
        }
    } else if (vedis_value_is_int(result)) {
        RETURN_LONG(vedis_value_to_int64(result));
    } else {
        RETURN_STRING(vedis_value_to_string(result, 0), 1);
    }
}

/*
 * TODO: serializer
 */

/*
 * [ Does not implement: Vedis Command ]
 * COPY
 * RAND
 * GETRANDMAX
 * RANDSTR
 * BASE64
 * BASE64_DEC
 * SOUNDEX
 * SIZE_FMT
 * GETCSV
 * STRIP_TAG
 * STR_SPLIT
 * TIME
 * DATE
 * OS
 * ECHO
 * PRINT
 * ABORT
 */

static zend_function_entry php_vedis_methods[] = {
    ZEND_ME(Vedis, __construct, arginfo_vedis___construct,
            ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    ZEND_ME(Vedis, get, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, set, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, setnx, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, del, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_MALIAS(Vedis, remove, del, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_MALIAS(Vedis, delete, del, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, exists, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, incr, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, incrby, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, decr, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, decrby, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, mget, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, getset, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, rename, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, append, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, strlen, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, mset, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, msetnx, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hset, arginfo_vedis_param3, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hsetnx, arginfo_vedis_param3, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hget, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hlen, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hdel, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hkeys, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hvals, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hgetall, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hexists, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hmset, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, hmget, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, lindex, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_MALIAS(Vedis, lget, lindex, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, lpop, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, lpush, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, llen, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_MALIAS(Vedis, lsize, llen, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, sadd, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, scard, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_MALIAS(Vedis, ssize, scard, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, sdiff, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, sinter, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, sismember, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, smembers, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, spop, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, srem, arginfo_vedis_param2, ZEND_ACC_PUBLIC)
    ZEND_MALIAS(Vedis, sremove, srem, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, speek, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, stop, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, commit, arginfo_vedis_none, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, rollback, arginfo_vedis_none, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, begin, arginfo_vedis_none, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, cmdlist, arginfo_vedis_none, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, tablelist, arginfo_vedis_none, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, credits, arginfo_vedis_none, ZEND_ACC_PUBLIC)
    ZEND_ME(Vedis, eval, arginfo_vedis_param1, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

static void
php_vedis_free_storage(void *object TSRMLS_DC)
{
    php_vedis_object_t *intern = (php_vedis_object_t *)object;

    if (!intern) {
        return;
    }

    if (intern->vedis) {
        if (!intern->vedis->is_persistent) {
            php_vedis_store_destroy(intern->vedis);
        }
    }

    zend_object_std_dtor(&intern->std TSRMLS_CC);
    efree(intern);
}

static zend_object_value
php_vedis_object_new_ex(zend_class_entry *ce, php_vedis_object_t **ptr TSRMLS_DC)
{
    php_vedis_object_t *intern;
    zend_object_value retval;
#if ZEND_MODULE_API_NO < 20100525
    zval *tmp;
#endif

    intern = emalloc(sizeof(php_vedis_object_t));
    memset(intern, 0, sizeof(php_vedis_object_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);

#if ZEND_MODULE_API_NO >= 20100525
    object_properties_init(&intern->std, ce);
#else
    zend_hash_copy(intern->std.properties, &ce->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
#endif

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_vedis_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_vedis_handlers;

    intern->vedis = NULL;

    return retval;
}

static zend_object_value
php_vedis_object_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_vedis_object_new_ex(ce, NULL TSRMLS_CC);
}

static int
php_vedis_class_register(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Vedis", php_vedis_methods);

    ce.create_object = php_vedis_object_new;

    php_vedis_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_vedis_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_vedis_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_vedis_handlers.clone_obj = NULL;

    return SUCCESS;
}

ZEND_RSRC_DTOR_FUNC(php_vedis_dtor)
{
    if (rsrc->ptr) {
        php_vedis_t *vedis = (php_vedis_t *)rsrc->ptr;
        php_vedis_store_destroy(vedis);
        rsrc->ptr = NULL;
    }
}

ZEND_MINIT_FUNCTION(vedis)
{
    REGISTER_INI_ENTRIES();

    le_vedis = zend_register_list_destructors_ex(NULL, php_vedis_dtor,
                                                 "Vedis persistent",
                                                 module_number);

    php_vedis_class_register(TSRMLS_C);


    return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(vedis)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

ZEND_MINFO_FUNCTION(vedis)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Vedis support", "enabled");
    php_info_print_table_row(2, "Version", VEDIS_VERSION);
    php_info_print_table_row(2, "Extension Version", VEDIS_EXT_VERSION);
    php_info_print_table_end();
}

zend_module_entry vedis_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "vedis",
    NULL,
    ZEND_MINIT(vedis),
    ZEND_MSHUTDOWN(vedis),
    NULL,
    NULL,
    ZEND_MINFO(vedis),
#if ZEND_MODULE_API_NO >= 20010901
    VEDIS_EXT_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VEDIS
ZEND_GET_MODULE(vedis)
#endif
