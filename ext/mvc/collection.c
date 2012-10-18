
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          Kenji Minamoto <kenji.minamoto@gmail.com>                     |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/fcall.h"
#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/operators.h"

PHP_METHOD(Phalcon_Mvc_Collection, __construct){

	zval *dependency_injector = NULL, *service_name;
	zval *manager;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &dependency_injector) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!dependency_injector) {
		PHALCON_INIT_NVAR(dependency_injector);
	} else {
		PHALCON_SEPARATE_PARAM(dependency_injector);
	}
	
	if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
		PHALCON_INIT_NVAR(dependency_injector);
		PHALCON_CALL_STATIC(dependency_injector, "phalcon\\di", "getdefault");
	}
	if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_collection_exception_ce, "A dependency injector container is required to obtain the services related to the ODM");
		return;
	}
	
	phalcon_update_property_zval(this_ptr, SL("_dependencyInjector"), dependency_injector TSRMLS_CC);
	
	PHALCON_INIT_VAR(service_name);
	ZVAL_STRING(service_name, "collectionManager", 1);
	
	PHALCON_INIT_VAR(manager);
	PHALCON_CALL_METHOD_PARAMS_1(manager, dependency_injector, "getshared", service_name, PH_NO_CHECK);
	PHALCON_CALL_METHOD_PARAMS_1_NORETURN(manager, "initialize", this_ptr, PH_NO_CHECK);
	
	PHALCON_MM_RESTORE();
}

/**
 * Sets the dependency injection container
 *
 * @param Phalcon\DI $dependencyInjector
 */
PHP_METHOD(Phalcon_Mvc_Collection, setDI){

	zval *dependency_injector;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &dependency_injector) == FAILURE) {
		RETURN_NULL();
	}

	phalcon_update_property_zval(this_ptr, SL("_dependencyInjector"), dependency_injector TSRMLS_CC);
	
}

/**
 * Returns the dependency injection container
 *
 * @return Phalcon\DI
 */
PHP_METHOD(Phalcon_Mvc_Collection, getDI){

	zval *dependency_injector;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(dependency_injector);
	phalcon_read_property(&dependency_injector, this_ptr, SL("_dependencyInjector"), PH_NOISY_CC);
	
	RETURN_CCTOR(dependency_injector);
}

/**
 * Sets the event manager
 *
 * @param Phalcon\Events\Manager $eventsManager
 */
PHP_METHOD(Phalcon_Mvc_Collection, setEventsManager){

	zval *events_manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &events_manager) == FAILURE) {
		RETURN_NULL();
	}

	phalcon_update_property_zval(this_ptr, SL("_eventsManager"), events_manager TSRMLS_CC);
	
}

/**
 * Returns the internal event manager
 *
 * @return Phalcon\Events\Manager
 */
PHP_METHOD(Phalcon_Mvc_Collection, getEventsManager){

	zval *events_manager;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(events_manager);
	phalcon_read_property(&events_manager, this_ptr, SL("_eventsManager"), PH_NOISY_CC);
	
	RETURN_CCTOR(events_manager);
}

/**
 *
 * @return array
 */
PHP_METHOD(Phalcon_Mvc_Collection, getReservedAttributes){

	zval *reserved = NULL;

	PHALCON_MM_GROW();

	PHALCON_OBSERVE_VAR(reserved);
	phalcon_read_static_property(&reserved, SL("phalcon\\mvc\\collection"), SL("_reserved") TSRMLS_CC);
	if (Z_TYPE_P(reserved) == IS_NULL) {
		PHALCON_INIT_NVAR(reserved);
		array_init(reserved);
		add_assoc_bool_ex(reserved, SS("_connectionService"), 1);
		add_assoc_bool_ex(reserved, SS("_connection"), 1);
		add_assoc_bool_ex(reserved, SS("_dependencyInjector"), 1);
		add_assoc_bool_ex(reserved, SS("_eventsManager"), 1);
		add_assoc_bool_ex(reserved, SS("_source"), 1);
		phalcon_update_static_property(SL("phalcon\\mvc\\collection"), SL("_reserved"), reserved TSRMLS_CC);
	}
	
	
	RETURN_CCTOR(reserved);
}

/**
 * Sets collection name which model should be mapped
 *
 * @param string $source
 * @return Phalcon\Mvc\Collection
 */
PHP_METHOD(Phalcon_Mvc_Collection, setSource){

	zval *source;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &source) == FAILURE) {
		RETURN_NULL();
	}

	phalcon_update_property_zval(this_ptr, SL("_source"), source TSRMLS_CC);
	
	RETURN_CTORW(this_ptr);
}

/**
 * Returns collection name mapped in the model
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Collection, getSource){

	zval *source = NULL, *class_name;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(source);
	phalcon_read_property(&source, this_ptr, SL("_source"), PH_NOISY_CC);
	if (!zend_is_true(source)) {
		PHALCON_INIT_VAR(class_name);
		phalcon_get_class(class_name, this_ptr TSRMLS_CC);
		
		PHALCON_INIT_NVAR(source);
		phalcon_uncamelize(source, class_name TSRMLS_CC);
		phalcon_update_property_zval(this_ptr, SL("_source"), source TSRMLS_CC);
	}
	
	
	RETURN_CCTOR(source);
}

PHP_METHOD(Phalcon_Mvc_Collection, setConnectionService){


	
}

PHP_METHOD(Phalcon_Mvc_Collection, getConnection){

	zval *connection = NULL, *dependency_injector, *connection_service;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(connection);
	phalcon_read_property(&connection, this_ptr, SL("_connection"), PH_NOISY_CC);
	if (Z_TYPE_P(connection) != IS_OBJECT) {
		PHALCON_INIT_VAR(dependency_injector);
		phalcon_read_property(&dependency_injector, this_ptr, SL("_dependencyInjector"), PH_NOISY_CC);
		if (Z_TYPE_P(dependency_injector) != IS_OBJECT) {
			PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_collection_exception_ce, "A dependency injector container is required to obtain the services related to the ODM");
			return;
		}
		
		PHALCON_INIT_VAR(connection_service);
		phalcon_read_property(&connection_service, this_ptr, SL("_connectionService"), PH_NOISY_CC);
		
		PHALCON_INIT_NVAR(connection);
		PHALCON_CALL_METHOD_PARAMS_1(connection, dependency_injector, "get", connection_service, PH_NO_CHECK);
		phalcon_update_property_zval(this_ptr, SL("_connection"), connection TSRMLS_CC);
	}
	
	
	RETURN_CCTOR(connection);
}

/**
 * Reads an attribute value by its name
 *
 * <code> echo $robot->readAttribute('name');</code>
 *
 * @param string $attribute
 * @return mixed
 */
PHP_METHOD(Phalcon_Mvc_Collection, readAttribute){

	zval *attribute, *attribute_value;
	int eval_int;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &attribute) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	eval_int = phalcon_isset_property_zval(this_ptr, attribute TSRMLS_CC);
	if (eval_int) {
		PHALCON_INIT_VAR(attribute_value);
		phalcon_read_property_zval(&attribute_value, this_ptr, attribute, PH_NOISY_CC);
		
		RETURN_CCTOR(attribute_value);
	}
	PHALCON_MM_RESTORE();
	RETURN_NULL();
}

/**
 * Writes an attribute value by its name
 *
 * <code>$robot->writeAttribute('name', 'Rosey');</code>
 *
 * @param string $attribute
 * @param mixed $value
 */
PHP_METHOD(Phalcon_Mvc_Collection, writeAttribute){

	zval *attribute, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &attribute, &value) == FAILURE) {
		RETURN_NULL();
	}

	phalcon_update_property_zval_zval(this_ptr, attribute, value TSRMLS_CC);
	
}

PHP_METHOD(Phalcon_Mvc_Collection, dumpResult){

	zval *model, *row, *cloned_model, *value = NULL, *key = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	char *hash_index;
	uint hash_index_len;
	ulong hash_num;
	int hash_type;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &model, &row) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (Z_TYPE_P(model) == IS_OBJECT) {
		PHALCON_INIT_VAR(cloned_model);
		if (phalcon_clone(cloned_model, model TSRMLS_CC) == FAILURE) {
			return;
		}
		
		if (!phalcon_valid_foreach(row TSRMLS_CC)) {
			return;
		}
		
		ah0 = Z_ARRVAL_P(row);
		zend_hash_internal_pointer_reset_ex(ah0, &hp0);
		
		ph_cycle_start_0:
		
			if (zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS) {
				goto ph_cycle_end_0;
			}
			
			PHALCON_GET_FOREACH_KEY(key, ah0, hp0);
			PHALCON_GET_FOREACH_VALUE(value);
			
			PHALCON_CALL_METHOD_PARAMS_2_NORETURN(cloned_model, "writeattribute", key, value, PH_NO_CHECK);
			
			zend_hash_move_forward_ex(ah0, &hp0);
			goto ph_cycle_start_0;
			
		ph_cycle_end_0:
		
		
		RETURN_CCTOR(cloned_model);
	}
	
	PHALCON_MM_RESTORE();
}

PHP_METHOD(Phalcon_Mvc_Collection, _getResultset){

	zval *params, *model, *connection, *unique, *source;
	zval *collection, *conditions = NULL, *row = NULL, *result, *rows;
	zval *limit, *sort, *models, *rows_array, *model_cloned = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	int eval_int;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzz", &params, &model, &connection, &unique) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(source);
	PHALCON_CALL_METHOD(source, model, "getsource", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(collection);
	phalcon_read_property_zval(&collection, connection, source, PH_NOISY_CC);
	eval_int = phalcon_array_isset_long(params, 0);
	if (eval_int) {
		PHALCON_INIT_VAR(conditions);
		phalcon_array_fetch_long(&conditions, params, 0, PH_NOISY_CC);
	} else {
		eval_int = phalcon_array_isset_string(params, SS("conditions"));
		if (eval_int) {
			PHALCON_INIT_NVAR(conditions);
			phalcon_array_fetch_string(&conditions, params, SL("conditions"), PH_NOISY_CC);
		} else {
			PHALCON_INIT_NVAR(conditions);
			array_init(conditions);
		}
	}
	
	if (PHALCON_IS_TRUE(unique)) {
		PHALCON_INIT_VAR(row);
		PHALCON_CALL_METHOD_PARAMS_1(row, collection, "findone", conditions, PH_NO_CHECK);
		
		PHALCON_INIT_VAR(result);
		PHALCON_CALL_SELF_PARAMS_2(result, this_ptr, "dumpresult", model, row);
		
		RETURN_CCTOR(result);
	}
	
	PHALCON_INIT_VAR(rows);
	PHALCON_CALL_METHOD_PARAMS_1(rows, collection, "find", conditions, PH_NO_CHECK);
	eval_int = phalcon_array_isset_string(params, SS("limit"));
	if (eval_int) {
		PHALCON_INIT_VAR(limit);
		phalcon_array_fetch_string(&limit, params, SL("limit"), PH_NOISY_CC);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(rows, "limit", limit, PH_NO_CHECK);
	}
	
	eval_int = phalcon_array_isset_string(params, SS("sort"));
	if (eval_int) {
		PHALCON_INIT_VAR(sort);
		phalcon_array_fetch_string(&sort, params, SL("sort"), PH_NOISY_CC);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(rows, "sort", sort, PH_NO_CHECK);
	}
	
	PHALCON_INIT_VAR(models);
	array_init(models);
	
	PHALCON_INIT_VAR(rows_array);
	PHALCON_CALL_FUNC_PARAMS_1(rows_array, "iterator_to_array", rows);
	
	if (!phalcon_valid_foreach(rows_array TSRMLS_CC)) {
		return;
	}
	
	ah0 = Z_ARRVAL_P(rows_array);
	zend_hash_internal_pointer_reset_ex(ah0, &hp0);
	
	ph_cycle_start_0:
	
		if (zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS) {
			goto ph_cycle_end_0;
		}
		
		PHALCON_GET_FOREACH_VALUE(row);
		
		PHALCON_INIT_NVAR(model_cloned);
		PHALCON_CALL_SELF_PARAMS_2(model_cloned, this_ptr, "dumpresult", model, row);
		phalcon_array_append(&models, model_cloned, PH_SEPARATE TSRMLS_CC);
		
		zend_hash_move_forward_ex(ah0, &hp0);
		goto ph_cycle_start_0;
		
	ph_cycle_end_0:
	
	
	RETURN_CTOR(models);
}

/**
 * Executes internal hooks before save a record
 *
 * @param Phalcon\DI $dependencyInjector
 * @param Phalcon\Mvc\Model\Metadata $metaData
 * @param boolean $disableEvents
 * @param boolean $exists
 * @param string $identityField
 * @return boolean
 */
PHP_METHOD(Phalcon_Mvc_Collection, _preSave){

	zval *dependency_injector, *meta_data, *disable_events;
	zval *exists, *identity_field, *event_name = NULL, *status = NULL;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzzz", &dependency_injector, &meta_data, &disable_events, &exists, &identity_field) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!zend_is_true(disable_events)) {
		PHALCON_INIT_VAR(event_name);
		ZVAL_STRING(event_name, "beforeValidation", 1);
		
		PHALCON_INIT_VAR(status);
		PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
		if (PHALCON_IS_FALSE(status)) {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
		
		if (!zend_is_true(exists)) {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "beforeValidationOnCreate", 1);
		} else {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "beforeValidationOnUpdate", 1);
		}
		
		PHALCON_INIT_NVAR(status);
		PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
		if (PHALCON_IS_FALSE(status)) {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
	}
	
	PHALCON_INIT_NVAR(event_name);
	ZVAL_STRING(event_name, "validation", 1);
	
	PHALCON_INIT_NVAR(status);
	PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
	if (PHALCON_IS_FALSE(status)) {
		if (!zend_is_true(disable_events)) {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "onValidationFails", 1);
			PHALCON_CALL_METHOD_PARAMS_1_NORETURN(this_ptr, "_callevent", event_name, PH_NO_CHECK);
		}
		PHALCON_MM_RESTORE();
		RETURN_FALSE;
	}
	
	if (!zend_is_true(disable_events)) {
		if (!zend_is_true(exists)) {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "afterValidationOnCreate", 1);
		} else {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "afterValidationOnUpdate", 1);
		}
		
		PHALCON_INIT_NVAR(status);
		PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
		if (PHALCON_IS_FALSE(status)) {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
		
		PHALCON_INIT_NVAR(event_name);
		ZVAL_STRING(event_name, "afterValidation", 1);
		
		PHALCON_INIT_NVAR(status);
		PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
		if (PHALCON_IS_FALSE(status)) {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
		
		PHALCON_INIT_NVAR(event_name);
		ZVAL_STRING(event_name, "beforeSave", 1);
		
		PHALCON_INIT_NVAR(status);
		PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
		if (PHALCON_IS_FALSE(status)) {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
		
		if (zend_is_true(exists)) {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "beforeUpdate", 1);
		} else {
			PHALCON_INIT_NVAR(event_name);
			ZVAL_STRING(event_name, "beforeCreate", 1);
		}
		
		PHALCON_INIT_NVAR(status);
		PHALCON_CALL_METHOD_PARAMS_1(status, this_ptr, "_calleventcancel", event_name, PH_NO_CHECK);
		if (PHALCON_IS_FALSE(status)) {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
	}
	
	PHALCON_MM_RESTORE();
	RETURN_TRUE;
}

PHP_METHOD(Phalcon_Mvc_Collection, save){

	zval *data, *reserved, *properties, *value = NULL, *key = NULL, *source;
	zval *connection, *collection, *success;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	char *hash_index;
	uint hash_index_len;
	ulong hash_num;
	int hash_type;
	int eval_int;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(data);
	array_init(data);
	
	PHALCON_INIT_VAR(reserved);
	PHALCON_CALL_METHOD(reserved, this_ptr, "getreservedattributes", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(properties);
	PHALCON_CALL_FUNC_PARAMS_1(properties, "get_object_vars", this_ptr);
	
	if (!phalcon_valid_foreach(properties TSRMLS_CC)) {
		return;
	}
	
	ah0 = Z_ARRVAL_P(properties);
	zend_hash_internal_pointer_reset_ex(ah0, &hp0);
	
	ph_cycle_start_0:
	
		if (zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS) {
			goto ph_cycle_end_0;
		}
		
		PHALCON_GET_FOREACH_KEY(key, ah0, hp0);
		PHALCON_GET_FOREACH_VALUE(value);
		
		eval_int = phalcon_array_isset(reserved, key);
		if (!eval_int) {
			phalcon_array_update_zval(&data, key, &value, PH_COPY | PH_SEPARATE TSRMLS_CC);
		}
		
		zend_hash_move_forward_ex(ah0, &hp0);
		goto ph_cycle_start_0;
		
	ph_cycle_end_0:
	
	PHALCON_INIT_VAR(source);
	PHALCON_CALL_METHOD(source, this_ptr, "getsource", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(connection);
	PHALCON_CALL_METHOD(connection, this_ptr, "getconnection", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(collection);
	phalcon_read_property_zval(&collection, connection, source, PH_NOISY_CC);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, collection, "save", data, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

PHP_METHOD(Phalcon_Mvc_Collection, findFirst){

	zval *parameters = NULL, *params = NULL, *class_name, *model;
	zval *connection, *unique, *resultset = NULL;
	zval *r0 = NULL;
	zval *p0[] = { NULL, NULL, NULL, NULL };
	zend_class_entry *ce0;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &parameters) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!parameters) {
		PHALCON_INIT_NVAR(parameters);
	}
	
	if (Z_TYPE_P(parameters) != IS_ARRAY) { 
		if (Z_TYPE_P(parameters) != IS_NULL) {
			PHALCON_INIT_VAR(params);
			array_init(params);
			phalcon_array_append(&params, parameters, PH_SEPARATE TSRMLS_CC);
		} else {
			PHALCON_INIT_NVAR(params);
			array_init(params);
		}
	} else {
		PHALCON_CPY_WRT(params, parameters);
	}
	
	PHALCON_INIT_VAR(class_name);
	PHALCON_CALL_FUNC(class_name, "get_called_class");
	ce0 = phalcon_fetch_class(class_name TSRMLS_CC);
	
	PHALCON_INIT_VAR(model);
	object_init_ex(model, ce0);
	PHALCON_CALL_METHOD_NORETURN(model, "__construct", PH_CHECK);
	
	PHALCON_INIT_VAR(connection);
	PHALCON_CALL_METHOD(connection, model, "getconnection", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(unique);
	ZVAL_BOOL(unique, 1);
	p0[0] = params;
	p0[1] = model;
	p0[2] = connection;
	p0[3] = unique;
	
	PHALCON_INIT_VAR(r0);
	PHALCON_CALL_SELF_PARAMS(r0, this_ptr, "_getresultset", 4, p0);
	PHALCON_CPY_WRT(resultset, r0);
	
	RETURN_CCTOR(resultset);
}

PHP_METHOD(Phalcon_Mvc_Collection, find){

	zval *parameters = NULL, *params = NULL, *class_name, *model;
	zval *connection, *unique, *resultset = NULL;
	zval *r0 = NULL;
	zval *p0[] = { NULL, NULL, NULL, NULL };
	zend_class_entry *ce0;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &parameters) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!parameters) {
		PHALCON_INIT_NVAR(parameters);
	}
	
	if (Z_TYPE_P(parameters) != IS_ARRAY) { 
		if (Z_TYPE_P(parameters) != IS_NULL) {
			PHALCON_INIT_VAR(params);
			array_init(params);
			phalcon_array_append(&params, parameters, PH_SEPARATE TSRMLS_CC);
		} else {
			PHALCON_INIT_NVAR(params);
			array_init(params);
		}
	} else {
		PHALCON_CPY_WRT(params, parameters);
	}
	
	PHALCON_INIT_VAR(class_name);
	PHALCON_CALL_FUNC(class_name, "get_called_class");
	ce0 = phalcon_fetch_class(class_name TSRMLS_CC);
	
	PHALCON_INIT_VAR(model);
	object_init_ex(model, ce0);
	PHALCON_CALL_METHOD_NORETURN(model, "__construct", PH_CHECK);
	
	PHALCON_INIT_VAR(connection);
	PHALCON_CALL_METHOD(connection, model, "getconnection", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(unique);
	ZVAL_BOOL(unique, 0);
	p0[0] = params;
	p0[1] = model;
	p0[2] = connection;
	p0[3] = unique;
	
	PHALCON_INIT_VAR(r0);
	PHALCON_CALL_SELF_PARAMS(r0, this_ptr, "_getresultset", 4, p0);
	PHALCON_CPY_WRT(resultset, r0);
	
	RETURN_CCTOR(resultset);
}

PHP_METHOD(Phalcon_Mvc_Collection, delete){

	zval *id, *connection, *source, *collection, *id_condition;
	zval *success;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(id);
	phalcon_read_property(&id, this_ptr, SL("_id"), PH_NOISY_CC);
	if (Z_TYPE_P(id) == IS_OBJECT) {
		PHALCON_INIT_VAR(connection);
		PHALCON_CALL_METHOD(connection, this_ptr, "getconnection", PH_NO_CHECK);
		
		PHALCON_INIT_VAR(source);
		PHALCON_CALL_METHOD(source, this_ptr, "getsource", PH_NO_CHECK);
		
		PHALCON_INIT_VAR(collection);
		phalcon_read_property_zval(&collection, connection, source, PH_NOISY_CC);
		
		PHALCON_INIT_VAR(id_condition);
		array_init(id_condition);
		phalcon_array_update_string(&id_condition, SL("_id"), &id, PH_COPY | PH_SEPARATE TSRMLS_CC);
		
		PHALCON_INIT_VAR(success);
		PHALCON_CALL_METHOD_PARAMS_1(success, collection, "remove", id_condition, PH_NO_CHECK);
		
		RETURN_CCTOR(success);
	}
	
	PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_collection_exception_ce, "The document cannot be removed because it doesn't exists");
	return;
}
