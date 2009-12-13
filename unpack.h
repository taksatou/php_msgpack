/*
 * MessagePack for PHP packing routine
 *
 * Copyright (C) 2009 Takayuki SATO
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#define MSGPACK_MAX_STACK_SIZE  (1024)
#include "unpack_define.h"

typedef struct unpack_user {
  int enable_array;
} unpack_user;


#define msgpack_unpack_struct(name) \
	struct template ## name

#define msgpack_unpack_func(ret, name) \
	static inline ret template ## name

#define msgpack_unpack_callback(name) \
	template_callback ## name

#define msgpack_unpack_object zval*

#define msgpack_unpack_user unpack_user


struct template_context;
typedef struct template_context template_context;

static inline msgpack_unpack_object template_callback_root(unpack_user* u)
{
    return NULL;
}

static inline int template_callback_int64(unpack_user* u, int64_t d, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  ZVAL_LONG(*o, d);
  return 0;
}

static inline int template_callback_uint16(unpack_user* u, uint16_t d, msgpack_unpack_object* o)
{
  return template_callback_int64(u, d, o);
}
static inline int template_callback_uint8(unpack_user* u, uint8_t d, msgpack_unpack_object* o)
{
  return template_callback_int64(u, d, o);
}

static inline int template_callback_uint32(unpack_user* u, uint32_t d, msgpack_unpack_object* o)
{
    return template_callback_int64(u, d, o);
}

static inline int template_callback_uint64(unpack_user* u, uint64_t d, msgpack_unpack_object* o)
{
  return template_callback_int64(u, d, o);
}

static inline int template_callback_int32(unpack_user* u, int32_t d, msgpack_unpack_object* o)
{
  return template_callback_int64(u, d, o);
}

static inline int template_callback_int16(unpack_user* u, int16_t d, msgpack_unpack_object* o)
{
  return template_callback_int64(u, d, o);
}

static inline int template_callback_int8(unpack_user* u, int8_t d, msgpack_unpack_object* o)
{
  return template_callback_int64(u, d, o);
}

static inline int template_callback_double(unpack_user* u, double d, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  ZVAL_DOUBLE(*o, d);
  return 0;
}

static inline int template_callback_float(unpack_user* u, float d, msgpack_unpack_object* o)
{
  return template_callback_double(u, d, o);
}

static inline int template_callback_nil(unpack_user* u, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  ZVAL_NULL(*o);
  return 0;
}

static inline int template_callback_true(unpack_user* u, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  ZVAL_BOOL(*o, 1);
  return 0;
}

static inline int template_callback_false(unpack_user* u, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  ZVAL_BOOL(*o, 0);
  return 0;
}

/* msgpack_pack don't serialize array to array. */
static inline int template_callback_array(unpack_user* u, unsigned int n, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  if (u->enable_array) {
    array_init(*o);
  } else {
    ZVAL_NULL(*o);
  }
  return 0;
}

static inline int template_callback_array_item(unpack_user* u, unsigned int current, msgpack_unpack_object* c, msgpack_unpack_object o)
{
  if (u->enable_array) {
    add_index_zval(*c, current, o);
  } else {
    ;
  }
  return 0;
}

static inline int template_callback_map(unpack_user* u, unsigned int n, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  array_init(*o);
  return 0;
}

static inline int template_callback_map_item(unpack_user* u, msgpack_unpack_object* c, msgpack_unpack_object k, msgpack_unpack_object v)
{
  if (Z_TYPE_P(k) == IS_STRING) {
    add_assoc_zval_ex(*c, Z_STRVAL_P(k), Z_STRLEN_P(k), v);
  } else {
    add_index_zval(*c, Z_LVAL_P(k), v);
  }
  return 0;
}

static inline int template_callback_raw(unpack_user* u, const char* b, const char* p, unsigned int l, msgpack_unpack_object* o)
{
  MAKE_STD_ZVAL(*o);
  ZVAL_STRINGL(*o, p, l, 1);
  return 0;
}

#include "unpack_template.h"
