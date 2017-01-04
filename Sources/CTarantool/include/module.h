/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

/****************************************************************************
 * this file contains modified version of module.h from tarantool-dev package
 *****************************************************************************/

#ifndef TARANTOOL_MODULE_H_INCLUDED
#define TARANTOOL_MODULE_H_INCLUDED

/*
 * Copyright 2010-2017, Tarantool AUTHORS, please see AUTHORS file.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stddef.h>
#include <errno.h>
#include <string.h> /* strerror(3) */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h> /* ssize_t for Apple */
#include <sys/types.h> /* ssize_t */

/** \cond public */
void (*sayfunc)(int, const char *, int, const char *,
const char *, ...);
/** \endcond public */
/** \cond public */

struct fiber;
/**
 * Fiber - contains information about fiber
 */

typedef int (*fiber_func)(va_list);

/**
 * Create a new fiber.
 *
 * Takes a fiber from fiber cache, if it's not empty.
 * Can fail only if there is not enough memory for
 * the fiber structure or fiber stack.
 *
 * The created fiber automatically returns itself
 * to the fiber cache when its "main" function
 * completes.
 *
 * \param name       string with fiber name
 * \param fiber_func func for run inside fiber
 *
 * \sa fiber_start
 */
struct fiber *
(*fiber_new)(const char *name, fiber_func f);

/**
 * Return control to another fiber and wait until it'll be woken.
 *
 * \sa fiber_wakeup
 */
void
(*fiber_yield)(void);

/**
 * Start execution of created fiber.
 *
 * \param callee fiber to start
 * \param ...    arguments to start the fiber with
 *
 * \sa fiber_new
 */
void
(*fiber_start)(struct fiber *callee, ...);

/**
 * Interrupt a synchronous wait of a fiber
 *
 * \param f fiber to be woken up
 */
void
(*fiber_wakeup)(struct fiber *f);

/**
 * Cancel the subject fiber. (set FIBER_IS_CANCELLED flag)
 *
 * If target fiber's flag FIBER_IS_CANCELLABLE set, then it would
 * be woken up (maybe prematurely). Then current fiber yields
 * until the target fiber is dead (or is woken up by
 * \sa fiber_wakeup).
 *
 * \param f fiber to be cancelled
 */
void
(*fiber_cancel)(struct fiber *f);

/**
 * Make it possible or not possible to wakeup the current
 * fiber immediately when it's cancelled.
 *
 * @param yesno status to set
 * @return previous state.
 */
bool
(*fiber_set_cancellable)(bool yesno);

/**
 * Set fiber to be joinable (false by default).
 * \param yesno status to set
 */
void
(*fiber_set_joinable)(struct fiber *fiber, bool yesno);

/**
 * Wait until the fiber is dead and then move its execution
 * status to the caller.
 * The fiber must not be detached (@sa fiber_set_joinable()).
 * @pre FIBER_IS_JOINABLE flag is set.
 *
 * \param f fiber to be woken up
 * \return fiber function ret code
 */
int
(*fiber_join)(struct fiber *f);

/**
 * Put the current fiber to sleep for at least 's' seconds.
 *
 * \param s time to sleep
 *
 * \note this is a cancellation point (\sa fiber_is_cancelled)
 */
void
(*fiber_sleep)(double s);

/**
 * Check current fiber for cancellation (it must be checked
 * manually).
 */
bool
(*fiber_is_cancelled)();

/**
 * Report loop begin time as double (cheap).
 */
double
(*fiber_time)(void);

/**
 * Report loop begin time as 64-bit int.
 */
uint64_t
(*fiber_time64)(void);

/**
 * Reschedule fiber to end of event loop cycle.
 */
void
(*fiber_reschedule)(void);

/**
 * Return slab_cache suitable to use with tarantool/small library
 */
struct slab_cache;
struct slab_cache *
(*cord_slab_cache)(void);

/** \endcond public */
/** \cond public */

enum {
    /** READ event */
    COIO_READ  = 0x1,
    /** WRITE event */
    COIO_WRITE = 0x2,
};

/**
 * Wait until READ or WRITE event on socket (\a fd). Yields.
 * \param fd - non-blocking socket file description
 * \param events - requested events to wait.
 * Combination of TNT_IO_READ | TNT_IO_WRITE bit flags.
 * \param timeoout - timeout in seconds.
 * \retval 0 - timeout
 * \retval >0 - returned events. Combination of TNT_IO_READ | TNT_IO_WRITE
 * bit flags.
 */
int
(*coio_wait)(int fd, int event, double timeout);

/**
 * Close the fd and wake any fiber blocked in
 * coio_wait() call on this fd.
 */
int
(*coio_close)(int fd);

/** \endcond public */
/** \cond public */

/**
 * Create new eio task with specified function and
 * arguments. Yield and wait until the task is complete
 * or a timeout occurs.
 *
 * This function doesn't throw exceptions to avoid double error
 * checking: in most cases it's also necessary to check the return
 * value of the called function and perform necessary actions. If
 * func sets errno, the errno is preserved across the call.
 *
 * @retval -1 and errno = ENOMEM if failed to create a task
 * @retval the function return (errno is preserved).
 *
 * @code
 *	static ssize_t openfile_cb(va_list ap)
 *	{
 *	         const char *filename = va_arg(ap);
 *	         int flags = va_arg(ap);
 *	         return open(filename, flags);
 *	}
 *
 *	if (coio_call(openfile_cb, 0.10, "/tmp/file", 0) == -1)
 *		// handle errors.
 *	...
 * @endcode
 */

struct addrinfo;

/**
 * Fiber-friendly version of getaddrinfo(3).
 *
 * @param host host name, i.e. "tarantool.org"
 * @param port service name, i.e. "80" or "http"
 * @param hints hints, see getaddrinfo(3)
 * @param res[out] result, see getaddrinfo(3)
 * @param timeout timeout
 * @retval  0 on success, please free @a res using freeaddrinfo(3).
 * @retval -1 on error, check diag.
 *            Please note that the return value is not compatible with
 *            getaddrinfo(3).
 * @sa getaddrinfo()
 */
int
(*coio_getaddrinfo)(const char *host, const char *port,
                 const struct addrinfo *hints, struct addrinfo **res,
                 double timeout);
/** \endcond public */
/** \cond public */

/**
 * Return true if there is an active transaction.
 */
bool
(*box_txn)(void);

/**
 * Begin a transaction in the current fiber.
 *
 * A transaction is attached to caller fiber, therefore one fiber can have
 * only one active transaction.
 *
 * @retval 0 - success
 * @retval -1 - failed, perhaps a transaction has already been
 * started
 */
int
(*box_txn_begin)(void);

/**
 * Commit the current transaction.
 * @retval 0 - success
 * @retval -1 - failed, perhaps a disk write failure.
 * started
 */
int
(*box_txn_commit)(void);

/**
 * Rollback the current transaction.
 * May fail if called from a nested
 * statement.
 */
int
(*box_txn_rollback)(void);

/**
 * Allocate memory on txn memory pool.
 * The memory is automatically deallocated when the transaction
 * is committed or rolled back.
 *
 * @retval NULL out of memory
 */
void *
(*box_txn_alloc)(size_t size);

/** \endcond public */
/** \cond public */

typedef struct tuple_format box_tuple_format_t;

/**
 * Tuple Format.
 *
 * Each Tuple has associated format (class). Default format is used to
 * create tuples which are not attach to any particular space.
 */
box_tuple_format_t *
(*box_tuple_format_default)(void);

/**
 * Tuple
 */
typedef struct tuple box_tuple_t;

/**
 * Allocate and initialize a new tuple from a raw MsgPack Array data.
 *
 * \param format tuple format.
 * Use box_tuple_format_default() to create space-independent tuple.
 * \param data tuple data in MsgPack Array format ([field1, field2, ...]).
 * \param end the end of \a data
 * \retval NULL on out of memory
 * \retval tuple otherwise
 * \pre data, end is valid MsgPack Array
 * \sa \code box.tuple.new(data) \endcode
 */
box_tuple_t *
(*box_tuple_new)(box_tuple_format_t *format, const char *data, const char *end);

/**
 * Increase the reference counter of tuple.
 *
 * Tuples are reference counted. All functions that return tuples guarantee
 * that the last returned tuple is refcounted internally until the next
 * call to API function that yields or returns another tuple.
 *
 * You should increase the reference counter before taking tuples for long
 * processing in your code. Such tuples will not be garbage collected even
 * if another fiber remove they from space. After processing please
 * decrement the reference counter using box_tuple_unref(), otherwise the
 * tuple will leak.
 *
 * \param tuple a tuple
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa box_tuple_unref()
 */
int
(*box_tuple_ref)(box_tuple_t *tuple);

/**
 * Decrease the reference counter of tuple.
 *
 * \param tuple a tuple
 * \sa box_tuple_ref()
 */
void
(*box_tuple_unref)(box_tuple_t *tuple);

/**
 * Return the number of fields in tuple (the size of MsgPack Array).
 * \param tuple a tuple
 */
uint32_t
(*box_tuple_field_count)(const box_tuple_t *tuple);

/**
 * Return the number of bytes used to store internal tuple data (MsgPack Array).
 * \param tuple a tuple
 */
size_t
(*box_tuple_bsize)(const box_tuple_t *tuple);

/**
 * Dump raw MsgPack data to the memory byffer \a buf of size \a size.
 *
 * Store tuple fields in the memory buffer.
 * \retval -1 on error.
 * \retval number of bytes written on success.
 * Upon successful return, the function returns the number of bytes written.
 * If buffer size is not enough then the return value is the number of bytes
 * which would have been written if enough space had been available.
 */
ssize_t
(*box_tuple_to_buf)(const box_tuple_t *tuple, char *buf, size_t size);

/**
 * Return the associated format.
 * \param tuple tuple
 * \return tuple_format
 */
box_tuple_format_t *
(*box_tuple_format)(const box_tuple_t *tuple);

/**
 * Return the raw tuple field in MsgPack format.
 *
 * The buffer is valid until next call to box_tuple_* functions.
 *
 * \param tuple a tuple
 * \param fieldno zero-based index in MsgPack array.
 * \retval NULL if i >= box_tuple_field_count(tuple)
 * \retval msgpack otherwise
 */
const char *
(*box_tuple_field)(const box_tuple_t *tuple, uint32_t fieldno);

/**
 * Tuple iterator
 */
typedef struct tuple_iterator box_tuple_iterator_t;

/**
 * Allocate and initialize a new tuple iterator. The tuple iterator
 * allow to iterate over fields at root level of MsgPack array.
 *
 * Example:
 * \code
 * box_tuple_iterator *it = box_tuple_iterator(tuple);
 * if (it == NULL) {
 *      // error handling using box_error_last()
 * }
 * const char *field;
 * while (field = box_tuple_next(it)) {
 *      // process raw MsgPack data
 * }
 *
 * // rewind iterator to first position
 * box_tuple_rewind(it);
 * assert(box_tuple_position(it) == 0);
 *
 * // rewind iterator to first position
 * field = box_tuple_seek(it, 3);
 * assert(box_tuple_position(it) == 4);
 *
 * box_iterator_free(it);
 * \endcode
 *
 * \post box_tuple_position(it) == 0
 */
box_tuple_iterator_t *
(*box_tuple_iterator)(box_tuple_t *tuple);

/**
 * Destroy and free tuple iterator
 */
void
(*box_tuple_iterator_free)(box_tuple_iterator_t *it);

/**
 * Return zero-based next position in iterator.
 * That is, this function return the field id of field that will be
 * returned by the next call to box_tuple_next(it). Returned value is zero
 * after initialization or rewind and box_tuple_field_count(tuple)
 * after the end of iteration.
 *
 * \param it tuple iterator
 * \returns position.
 */
uint32_t
(*box_tuple_position)(box_tuple_iterator_t *it);

/**
 * Rewind iterator to the initial position.
 *
 * \param it tuple iterator
 * \post box_tuple_position(it) == 0
 */
void
(*box_tuple_rewind)(box_tuple_iterator_t *it);

/**
 * Seek the tuple iterator.
 *
 * The returned buffer is valid until next call to box_tuple_* API.
 * Requested fieldno returned by next call to box_tuple_next(it).
 *
 * \param it tuple iterator
 * \param fieldno - zero-based position in MsgPack array.
 * \post box_tuple_position(it) == fieldno if returned value is not NULL
 * \post box_tuple_position(it) == box_tuple_field_count(tuple) if returned
 * value is NULL.
 */
const char *
(*box_tuple_seek)(box_tuple_iterator_t *it, uint32_t fieldno);

/**
 * Return the next tuple field from tuple iterator.
 * The returned buffer is valid until next call to box_tuple_* API.
 *
 * \param it tuple iterator.
 * \retval NULL if there are no more fields.
 * \retval MsgPack otherwise
 * \pre box_tuple_position(it) is zerod-based id of returned field
 * \post box_tuple_position(it) == box_tuple_field_count(tuple) if returned
 * value is NULL.
 */
const char *
(*box_tuple_next)(box_tuple_iterator_t *it);

box_tuple_t *
(*box_tuple_update)(const box_tuple_t *tuple, const char *expr, const
                 char *expr_end);

box_tuple_t *
(*box_tuple_upsert)(const box_tuple_t *tuple, const char *expr, const
                 char *expr_end);

char *
(*box_tuple_extract_key)(const box_tuple_t *tuple, uint32_t space_id,
                      uint32_t index_id, uint32_t *key_size);

/** \endcond public */
/** \cond public */
enum {
    /** Start of the reserved range of system spaces. */
    BOX_SYSTEM_ID_MIN = 256,
    /** Space id of _schema. */
    BOX_SCHEMA_ID = 272,
    /** Space id of _space. */
    BOX_SPACE_ID = 280,
    /** Space id of _vspace view. */
    BOX_VSPACE_ID = 281,
    /** Space id of _index. */
    BOX_INDEX_ID = 288,
    /** Space id of _vindex view. */
    BOX_VINDEX_ID = 289,
    /** Space id of _func. */
    BOX_FUNC_ID = 296,
    /** Space id of _vfunc view. */
    BOX_VFUNC_ID = 297,
    /** Space id of _user. */
    BOX_USER_ID = 304,
    /** Space id of _vuser view. */
    BOX_VUSER_ID = 305,
    /** Space id of _priv. */
    BOX_PRIV_ID = 312,
    /** Space id of _vpriv view. */
    BOX_VPRIV_ID = 313,
    /** Space id of _cluster. */
    BOX_CLUSTER_ID = 320,
    /** Space id of _vinyl_space. */
    BOX_VINYL_RUN_ID = 328,
    /** Space id of _vinyl_page. */
    BOX_VINYL_PAGE_ID = 326,
    /** Space id of _vinyl_rows. */
    BOX_VINYL_ROW_INDEX_ID = 327,
    /** End of the reserved range of system spaces. */
    BOX_SYSTEM_ID_MAX = 511,
    BOX_ID_NIL = 2147483647
};
/** \endcond public */
/** \cond public */

/*
 * Opaque structure passed to the stored C procedure
 */
typedef struct box_function_ctx box_function_ctx_t;

/**
 * Return a tuple from stored C procedure.
 *
 * Returned tuple is automatically reference counted by Tarantool.
 *
 * \param ctx an opaque structure passed to the stored C procedure by
 * Tarantool
 * \param tuple a tuple to return
 * \retval -1 on error (perhaps, out of memory; check box_error_last())
 * \retval 0 otherwise
 */
int
(*box_return_tuple)(box_function_ctx_t *ctx, box_tuple_t *tuple);

/**
 * Find space id by name.
 *
 * This function performs SELECT request to _vspace system space.
 * \param name space name
 * \param len length of \a name
 * \retval BOX_ID_NIL on error or if not found (check box_error_last())
 * \retval space_id otherwise
 * \sa box_index_id_by_name
 */
uint32_t
(*box_space_id_by_name)(const char *name, uint32_t len);

/**
 * Find index id by name.
 *
 * This function performs SELECT request to _vindex system space.
 * \param space_id space identifier
 * \param name index name
 * \param len length of \a name
 * \retval BOX_ID_NIL on error or if not found (check box_error_last())
 * \retval index_id otherwise
 * \sa box_space_id_by_name
 */
uint32_t
(*box_index_id_by_name)(uint32_t space_id, const char *name, uint32_t len);

/**
 * Execute an INSERT request.
 *
 * \param space_id space identifier
 * \param tuple encoded tuple in MsgPack Array format ([ field1, field2, ...])
 * \param tuple_end end of @a tuple
 * \param[out] result a new tuple. Can be set to NULL to discard result.
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id]:insert(tuple) \endcode
 */
int
(*box_insert)(uint32_t space_id, const char *tuple, const char *tuple_end,
           box_tuple_t **result);

/**
 * Execute an REPLACE request.
 *
 * \param space_id space identifier
 * \param tuple encoded tuple in MsgPack Array format ([ field1, field2, ...])
 * \param tuple_end end of @a tuple
 * \param[out] result a new tuple. Can be set to NULL to discard result.
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id]:replace(tuple) \endcode
 */
int
(*box_replace)(uint32_t space_id, const char *tuple, const char *tuple_end,
            box_tuple_t **result);

/**
 * Execute an DELETE request.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * \param key_end the end of encoded \a key.
 * \param[out] result an old tuple. Can be set to NULL to discard result.
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id].index[index_id]:delete(key) \endcode
 */
int
(*box_delete)(uint32_t space_id, uint32_t index_id, const char *key,
           const char *key_end, box_tuple_t **result);

/**
 * Execute an UPDATE request.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * \param key_end the end of encoded \a key.
 * \param ops encoded operations in MsgPack Arrat format, e.g.
 * [ [ '=', fieldno,  value ],  ['!', 2, 'xxx'] ]
 * \param ops_end the end of encoded \a ops
 * \param index_base 0 if fieldnos in update operations are zero-based
 * indexed (like C) or 1 if for one-based indexed field ids (like Lua).
 * \param[out] result a new tuple. Can be set to NULL to discard result.
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id].index[index_id]:update(key, ops) \endcode
 * \sa box_upsert()
 */
int
(*box_update)(uint32_t space_id, uint32_t index_id, const char *key,
           const char *key_end, const char *ops, const char *ops_end,
           int index_base, box_tuple_t **result);

/**
 * Execute an UPSERT request.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param ops encoded operations in MsgPack Arrat format, e.g.
 * [ [ '=', fieldno,  value ],  ['!', 2, 'xxx'] ]
 * \param ops_end the end of encoded \a ops
 * \param tuple encoded tuple in MsgPack Array format ([ field1, field2, ...])
 * \param tuple_end end of @a tuple
 * \param index_base 0 if fieldnos in update operations are zero-based
 * indexed (like C) or 1 if for one-based indexed field ids (like Lua).
 * \param[out] result a new tuple. Can be set to NULL to discard result.
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id].index[index_id]:update(key, ops) \endcode
 * \sa box_update()
 */
int
(*box_upsert)(uint32_t space_id, uint32_t index_id, const char *tuple,
           const char *tuple_end, const char *ops, const char *ops_end,
           int index_base, box_tuple_t **result);

/**
 * Truncate space.
 *
 * \param space_id space identifier
 */
int
(*box_truncate)(uint32_t space_id);

/** \endcond public */
/** \cond public */
/** A space iterator */
typedef struct iterator box_iterator_t;

/**
 * Controls how to iterate over tuples in an index.
 * Different index types support different iterator types.
 * For example, one can start iteration from a particular value
 * (request key) and then retrieve all tuples where keys are
 * greater or equal (= GE) to this key.
 *
 * If iterator type is not supported by the selected index type,
 * iterator constructor must fail with ER_UNSUPPORTED. To be
 * selectable for primary key, an index must support at least
 * ITER_EQ and ITER_GE types.
 *
 * NULL value of request key corresponds to the first or last
 * key in the index, depending on iteration direction.
 * (first key for GE and GT types, and last key for LE and LT).
 * Therefore, to iterate over all tuples in an index, one can
 * use ITER_GE or ITER_LE iteration types with start key equal
 * to NULL.
 * For ITER_EQ, the key must not be NULL.
 */

enum iterator_type {
    /* ITER_EQ must be the first member for request_create  */
    ITER_EQ               =  0, /* key == x ASC order                  */
    ITER_REQ              =  1, /* key == x DESC order                 */
    ITER_ALL              =  2, /* all tuples                          */
    ITER_LT               =  3, /* key <  x                            */
    ITER_LE               =  4, /* key <= x                            */
    ITER_GE               =  5, /* key >= x                            */
    ITER_GT               =  6, /* key >  x                            */
    ITER_BITS_ALL_SET     =  7, /* all bits from x are set in key      */
    ITER_BITS_ANY_SET     =  8, /* at least one x's bit is set         */
    ITER_BITS_ALL_NOT_SET =  9, /* all bits are not set                */
    ITER_OVERLAPS         = 10, /* key overlaps x                      */
    ITER_NEIGHBOR         = 11, /* typles in distance ascending order from specified point */
    iterator_type_MAX     = ITER_NEIGHBOR + 1
};

/**
 * Allocate and initialize iterator for space_id, index_id.
 *
 * A returned iterator must be destroyed by box_iterator_free().
 *
 * \param space_id space identifier.
 * \param index_id index identifier.
 * \param type \link iterator_type iterator type \endlink
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * \param key_end the end of encoded \a key
 * \retval NULL on error (check box_error_last())
 * \retval iterator otherwise
 * \sa box_iterator_next()
 * \sa box_iterator_free()
 */
box_iterator_t *
(*box_index_iterator)(uint32_t space_id, uint32_t index_id, int type,
                   const char *key, const char *key_end);
/**
 * Retrive the next item from the \a iterator.
 *
 * \param iterator an iterator returned by box_index_iterator().
 * \param[out] result a tuple or NULL if there is no more data.
 * \retval -1 on error (check box_error_last() for details)
 * \retval 0 on success. The end of data is not an error.
 */
int
(*box_iterator_next)(box_iterator_t *iterator, box_tuple_t **result);

/**
 * Destroy and deallocate iterator.
 *
 * \param iterator an interator returned by box_index_iterator()
 */
void
(*box_iterator_free)(box_iterator_t *iterator);

/** \endcond public */
/** \cond public */

/**
 * Return the number of element in the index.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \retval -1 on error (check box_error_last())
 * \retval >= 0 otherwise
 */
ssize_t
(*box_index_len)(uint32_t space_id, uint32_t index_id);

/**
 * Return the number of bytes used in memory by the index.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \retval -1 on error (check box_error_last())
 * \retval >= 0 otherwise
 */
ssize_t
(*box_index_bsize)(uint32_t space_id, uint32_t index_id);

/**
 * Return a random tuple from the index (useful for statistical analysis).
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param rnd random seed
 * \param[out] result a tuple or NULL if index is empty
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id].index[index_id]:random(rnd) \endcode
 */
int
(*box_index_random)(uint32_t space_id, uint32_t index_id, uint32_t rnd,
                 box_tuple_t **result);

/**
 * Get a tuple from index by the key.
 *
 * Please note that this function works much more faster than
 * box_select() or box_index_iterator() + box_iterator_next().
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * \param key_end the end of encoded \a key
 * \param[out] result a tuple or NULL if index is empty
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \pre key != NULL
 * \sa \code box.space[space_id].index[index_id]:get(key) \endcode
 */
int
(*box_index_get)(uint32_t space_id, uint32_t index_id, const char *key,
              const char *key_end, box_tuple_t **result);

/**
 * Return a first (minimal) tuple matched the provided key.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * If NULL then equvivalent to an empty array.
 * \param key_end the end of encoded \a key.
 * Must be NULL if \a key is NULL.
 * \param[out] result a tuple or NULL if index is empty
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id].index[index_id]:min(key) \endcode
 */
int
(*box_index_min)(uint32_t space_id, uint32_t index_id, const char *key,
              const char *key_end, box_tuple_t **result);

/**
 * Return a last (maximal) tuple matched the provided key.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * If NULL then equvivalent to an empty array.
 * \param key_end the end of encoded \a key.
 * Must be NULL if \a key is NULL.
 * \param[out] result a tuple or NULL if index is empty
 * \retval -1 on error (check box_error_last())
 * \retval 0 on success
 * \sa \code box.space[space_id].index[index_id]:max(key) \endcode
 */
int
(*box_index_max)(uint32_t space_id, uint32_t index_id, const char *key,
              const char *key_end, box_tuple_t **result);

/**
 * Count the number of tuple matched the provided key.
 *
 * \param space_id space identifier
 * \param index_id index identifier
 * \param type iterator type - enum \link iterator_type \endlink
 * \param key encoded key in MsgPack Array format ([part1, part2, ...]).
 * If NULL then equvivalent to an empty array.
 * \param key_end the end of encoded \a key.
 * Must be NULL if \a key is NULL.
 * \retval -1 on error (check box_error_last())
 * \retval >=0 on success
 * \sa \code box.space[space_id].index[index_id]:count(key,
 *     { iterator = type }) \endcode
 */
ssize_t
(*box_index_count)(uint32_t space_id, uint32_t index_id, int type,
                const char *key, const char *key_end);

/** \endcond public */
/** \cond public */

struct error;
/**
 * Error - contains information about error.
 */
typedef struct error box_error_t;

/**
 * Return the error type, e.g. "ClientError", "SocketError", etc.
 * \param error
 * \return not-null string
 */
const char *
(*box_error_type)(const box_error_t *error);

/**
 * Return IPROTO error code
 * \param error error
 * \return enum box_error_code
 */
uint32_t
(*box_error_code)(const box_error_t *error);

/**
 * Return the error message
 * \param error error
 * \return not-null string
 */
const char *
(*box_error_message)(const box_error_t *error);

/**
 * Get the information about the last API call error.
 *
 * The Tarantool error handling works most like libc's errno. All API calls
 * return -1 or NULL in the event of error. An internal pointer to
 * box_error_t type is set by API functions to indicate what went wrong.
 * This value is only significant if API call failed (returned -1 or NULL).
 *
 * Successful function can also touch the last error in some
 * cases. You don't have to clear the last error before calling
 * API functions. The returned object is valid only until next
 * call to **any** API function.
 *
 * You must set the last error using box_error_set() in your stored C
 * procedures if you want to return a custom error message.
 * You can re-throw the last API error to IPROTO client by keeping
 * the current value and returning -1 to Tarantool from your
 * stored procedure.
 *
 * \return last error.
 */
box_error_t *
(*box_error_last)(void);

/**
 * Clear the last error.
 */
void
(*box_error_clear)(void);

/** \endcond public */
/** \cond public */

/**
 * A lock for cooperative multitasking environment
 */
typedef struct box_latch box_latch_t;

/**
 * Allocate and initialize the new latch.
 * \returns latch
 */
box_latch_t*
(*box_latch_new)(void);

/**
 * Destroy and free the latch.
 * \param latch latch
 */
void
(*box_latch_delete)(box_latch_t *latch);

/**
 * Lock a latch. Waits indefinitely until the current fiber can gain access to
 * the latch.
 *
 * \param latch a latch
 */
void
(*box_latch_lock)(box_latch_t *latch);

/**
 * Try to lock a latch. Return immediately if the latch is locked.
 * \param latch a latch
 * \retval 0 - success
 * \retval 1 - the latch is locked.
 */
int
(*box_latch_trylock)(box_latch_t *latch);

/**
 * Unlock a latch. The fiber calling this function must
 * own the latch.
 *
 * \param latch a ltach
 */
void
(*box_latch_unlock)(box_latch_t *latch);

/** \endcond public */
/** \cond public */

double (*clock_realtime)(void);
double (*clock_monotonic)(void);
double (*clock_process)(void);
double (*clock_thread)(void);

uint64_t (*clock_realtime64)(void);
uint64_t (*clock_monotonic64)(void);
uint64_t (*clock_process64)(void);
uint64_t (*clock_thread64)(void);

/** \endcond public */
enum box_error_code { ER_UNKNOWN, ER_ILLEGAL_PARAMS, ER_MEMORY_ISSUE, ER_TUPLE_FOUND, ER_TUPLE_NOT_FOUND, ER_UNSUPPORTED, ER_NONMASTER, ER_READONLY, ER_INJECTION, ER_CREATE_SPACE, ER_SPACE_EXISTS, ER_DROP_SPACE, ER_ALTER_SPACE, ER_INDEX_TYPE, ER_MODIFY_INDEX, ER_LAST_DROP, ER_TUPLE_FORMAT_LIMIT, ER_DROP_PRIMARY_KEY, ER_KEY_PART_TYPE, ER_EXACT_MATCH, ER_INVALID_MSGPACK, ER_PROC_RET, ER_TUPLE_NOT_ARRAY, ER_FIELD_TYPE, ER_FIELD_TYPE_MISMATCH, ER_SPLICE, ER_UPDATE_ARG_TYPE, ER_TUPLE_IS_TOO_LONG, ER_UNKNOWN_UPDATE_OP, ER_UPDATE_FIELD, ER_FIBER_STACK, ER_KEY_PART_COUNT, ER_PROC_LUA, ER_NO_SUCH_PROC, ER_NO_SUCH_TRIGGER, ER_NO_SUCH_INDEX, ER_NO_SUCH_SPACE, ER_NO_SUCH_FIELD, ER_EXACT_FIELD_COUNT, ER_INDEX_FIELD_COUNT, ER_WAL_IO, ER_MORE_THAN_ONE_TUPLE, ER_ACCESS_DENIED, ER_CREATE_USER, ER_DROP_USER, ER_NO_SUCH_USER, ER_USER_EXISTS, ER_PASSWORD_MISMATCH, ER_UNKNOWN_REQUEST_TYPE, ER_UNKNOWN_SCHEMA_OBJECT, ER_CREATE_FUNCTION, ER_NO_SUCH_FUNCTION, ER_FUNCTION_EXISTS, ER_FUNCTION_ACCESS_DENIED, ER_FUNCTION_MAX, ER_SPACE_ACCESS_DENIED, ER_USER_MAX, ER_NO_SUCH_ENGINE, ER_RELOAD_CFG, ER_CFG, ER_VINYL, ER_LOCAL_SERVER_IS_NOT_ACTIVE, ER_UNKNOWN_SERVER, ER_CLUSTER_ID_MISMATCH, ER_INVALID_UUID, ER_CLUSTER_ID_IS_RO, ER_SERVER_ID_MISMATCH, ER_SERVER_ID_IS_RESERVED, ER_INVALID_ORDER, ER_MISSING_REQUEST_FIELD, ER_IDENTIFIER, ER_DROP_FUNCTION, ER_ITERATOR_TYPE, ER_REPLICA_MAX, ER_INVALID_XLOG, ER_INVALID_XLOG_NAME, ER_INVALID_XLOG_ORDER, ER_NO_CONNECTION, ER_TIMEOUT, ER_ACTIVE_TRANSACTION, ER_NO_ACTIVE_TRANSACTION, ER_CROSS_ENGINE_TRANSACTION, ER_NO_SUCH_ROLE, ER_ROLE_EXISTS, ER_CREATE_ROLE, ER_INDEX_EXISTS, ER_TUPLE_REF_OVERFLOW, ER_ROLE_LOOP, ER_GRANT, ER_PRIV_GRANTED, ER_ROLE_GRANTED, ER_PRIV_NOT_GRANTED, ER_ROLE_NOT_GRANTED, ER_MISSING_SNAPSHOT, ER_CANT_UPDATE_PRIMARY_KEY, ER_UPDATE_INTEGER_OVERFLOW, ER_GUEST_USER_PASSWORD, ER_TRANSACTION_CONFLICT, ER_UNSUPPORTED_ROLE_PRIV, ER_LOAD_FUNCTION, ER_FUNCTION_LANGUAGE, ER_RTREE_RECT, ER_PROC_C, ER_UNKNOWN_RTREE_INDEX_DISTANCE_TYPE, ER_PROTOCOL, ER_UPSERT_UNIQUE_SECONDARY_KEY, ER_WRONG_INDEX_RECORD, ER_WRONG_INDEX_PARTS, ER_WRONG_INDEX_OPTIONS, ER_WRONG_SCHEMA_VERSION, ER_SLAB_ALLOC_MAX, ER_WRONG_SPACE_OPTIONS, ER_UNSUPPORTED_INDEX_FEATURE, ER_VIEW_IS_RO, ER_SERVER_UUID_MISMATCH, ER_SYSTEM, ER_LOADING, ER_CONNECTION_TO_SELF, ER_KEY_PART_IS_TOO_LONG, ER_COMPRESSION, ER_SNAPSHOT_IN_PROGRESS, ER_SUB_STMT_MAX, ER_COMMIT_IN_SUB_STMT, ER_ROLLBACK_IN_SUB_STMT, ER_DECOMPRESSION, ER_INVALID_XLOG_TYPE, ER_INVALID_RUN_ID, ER_ALREADY_RUNNING, box_error_code_MAX };

#endif /* TARANTOOL_MODULE_H_INCLUDED */
