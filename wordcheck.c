/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 310447 2011-04-23 21:14:10Z bjori $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_wordcheck.h"

#include "wordcheck/platform.h"
#include "wordcheck/table.h"
#include "wordcheck/mm.h"
#include "wordcheck/hash.h"
#include "wordcheck/mmtable.h"
#include "wordcheck/docs.h"
#include "wordcheck/check.h"

static int phpwcstatus = 1;
static wcMM *phpwcMM;
static wcmmTable *phpwcmmtable;

/* If you declare any globals in php_wordcheck.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(wordcheck)
*/

/* True global resources - no need for thread safety here */
static int le_wordcheck;

/* {{{ wordcheck_functions[]
 *
 * Every user visible function must have an entry in wordcheck_functions[].
 */
const zend_function_entry wordcheck_functions[] = {
    PHP_FE(wordcheck_reset, NULL)
	PHP_FE(wordcheck_filter, NULL)
	    PHP_FE(wordcheck_set_replace_op, NULL)
		PHP_FE(wordcheck_set_replace_len, NULL){NULL, NULL, NULL}  //兼容5.3.8之前版本
    //PHP_FE_END	/* Must be the last line in wordcheck_functions[] */
};
/* }}} */

/* {{{ wordcheck_module_entry
 */
zend_module_entry wordcheck_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "wordcheck",
    wordcheck_functions,
    PHP_MINIT(wordcheck),
    PHP_MSHUTDOWN(wordcheck),
    PHP_RINIT(wordcheck),     /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(wordcheck), /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(wordcheck),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES};
/* }}} */

#ifdef COMPILE_DL_WORDCHECK
ZEND_GET_MODULE(wordcheck)
#endif

PHP_INI_BEGIN()
PHP_INI_ENTRY("wordcheck.enable", "0", PHP_INI_ALL, NULL)       /* 是否启用 */
PHP_INI_ENTRY("wordcheck.deny_docs", "deny", PHP_INI_ALL, NULL) /* 阻止字典 */
PHP_INI_END()

/* {{{ php_wordcheck_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_wordcheck_init_globals(zend_wordcheck_globals *wordcheck_globals)
{
	wordcheck_globals->global_value = 0;
	wordcheck_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(wordcheck)
{
    REGISTER_INI_ENTRIES();

    long enable = INI_INT("wordcheck.enable");
    char *deny_docs = INI_STR("wordcheck.deny_docs");

    if (enable == 1) {
	if (wordcheck_mm_fetch(&phpwcMM, deny_docs) == 0) {
	    wordcheck_mmtable_fetch(phpwcMM, &phpwcmmtable);
	    phpwcstatus = 0;
	}
    }

    /* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(wordcheck)
{
    if (phpwcstatus == 0) {
	wordcheck_mm_munmap(phpwcMM);
    }  //解除映射关系

    UNREGISTER_INI_ENTRIES();

    /* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(wordcheck)
{
    if (php_wordcheck_reset() == FAILURE) {
	wordcheck_reset_replace_op();
	wordcheck_reset_replace_len();
    }
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(wordcheck)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(wordcheck)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "wordcheck support", "enabled");
    php_info_print_table_row(2, "Author", "edwin.lzh@gmail.com");
    php_info_print_table_row(2, "Version", "0.13");
    php_info_print_table_row(2, "Status", (phpwcstatus == 1) ? "disable" : "enable");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

int php_wordcheck_reset(void)
{
    if (wordcheck_set_replace_op("*") == SUCCESS && wordcheck_set_replace_len(3) == SUCCESS) {
	return SUCCESS;
    } else {
	return FAILURE;
    }
}

PHP_FUNCTION(wordcheck_reset)
{
    if (php_wordcheck_reset() == SUCCESS) {
	RETVAL_TRUE;
    } else {
	RETVAL_FALSE;
    }
}

PHP_FUNCTION(wordcheck_set_replace_op)
{
    char *arg = NULL;
    int arg_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
	return;
    }

    if (wordcheck_set_replace_op(arg) == SUCCESS) {
	RETVAL_TRUE;
    } else {
	RETVAL_FALSE;
    }
}

PHP_FUNCTION(wordcheck_set_replace_len)
{
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", len) == FAILURE) {
	return;
    }

    if (wordcheck_set_replace_len(len) == SUCCESS) {
	RETVAL_TRUE;
    } else {
	RETVAL_FALSE;
    }
}

PHP_FUNCTION(wordcheck_filter)
{
    char *arg = NULL;
    int arg_len, len;
    char *strg;
    zval *arr = NULL;
    wcList *list;
    wcList *lt;
    int key = 0;
    int num = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a!", &arg, &arg_len, &arr) == FAILURE) {
	return;
    }

    if (phpwcstatus != 0) {
	RETVAL_FALSE;
	return;
    }

    if (arr == NULL) {
	num = wordcheck_mm_check(phpwcMM, phpwcmmtable, arg, arg_len, &strg, &len, NULL);
    } else {
	num = wordcheck_mm_check(phpwcMM, phpwcmmtable, arg, arg_len, &strg, &len, &list);
	array_init(arr);
	zval *words;
	while (wordcheck_list_get_current(list, &lt) == SUCCESS) {
	    wcResult *res = (wcResult *)lt->val;
	    //printf("-->禁词: %s\n", res->string);
	    //printf("-->起始: %d\n", res->start);
	    //printf("-->长度: %d\n\n", res->len);
	    MAKE_STD_ZVAL(words);
	    array_init(words);
	    add_assoc_string(words, "word", res->string, strlen(res->string));

	    zval *info;
	    MAKE_STD_ZVAL(info);
	    array_init(info);
	    add_assoc_long(info, "weight", res->info->weight);

	    add_assoc_zval(words, "info", info);
	    add_assoc_long(words, "start", res->start);
	    add_assoc_long(words, "length", res->len);

	    add_index_zval(arr, key, words);

	    key++;
	    wordcheck_list_next_item(&list);
	}
    }
    RETVAL_TRUE;
    RETURN_STRINGL(strg, len, 1);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
