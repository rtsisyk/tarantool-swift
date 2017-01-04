/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

#include <tarantool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void __attribute__ ((constructor)) tarantool_module_init(void);

void resolve(void *handle, const char *symbol, void **func) {
    *func = dlsym(handle, symbol);
    if (!func) {
        printf("can't resolve %s", symbol);
        dlclose(handle);
        exit(1);
    }
}

void tarantool_module_init() {
    void* handle = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        perror("dlopen error");
        exit(1);
    }

    resolve(handle, "sayfunc", (void**)&sayfunc);
    resolve(handle, "fiber_new", (void**)&fiber_new);
    resolve(handle, "fiber_yield", (void**)&fiber_yield);
    resolve(handle, "fiber_start", (void**)&fiber_start);
    resolve(handle, "fiber_wakeup", (void**)&fiber_wakeup);
    resolve(handle, "fiber_cancel", (void**)&fiber_cancel);
    resolve(handle, "fiber_set_cancellable", (void**)&fiber_set_cancellable);
    resolve(handle, "fiber_set_joinable", (void**)&fiber_set_joinable);
    resolve(handle, "fiber_join", (void**)&fiber_join);
    resolve(handle, "fiber_sleep", (void**)&fiber_sleep);
    resolve(handle, "fiber_is_cancelled", (void**)&fiber_is_cancelled);
    resolve(handle, "fiber_time", (void**)&fiber_time);
    resolve(handle, "fiber_time64", (void**)&fiber_time64);
    resolve(handle, "fiber_reschedule", (void**)&fiber_reschedule);
    resolve(handle, "cord_slab_cache", (void**)&cord_slab_cache);
    resolve(handle, "coio_wait", (void**)&coio_wait);
    resolve(handle, "coio_close", (void**)&coio_close);
    resolve(handle, "coio_getaddrinfo", (void**)&coio_getaddrinfo);
    resolve(handle, "box_txn", (void**)&box_txn);
    resolve(handle, "box_txn_begin", (void**)&box_txn_begin);
    resolve(handle, "box_txn_commit", (void**)&box_txn_commit);
    resolve(handle, "box_txn_rollback", (void**)&box_txn_rollback);
    resolve(handle, "box_txn_alloc", (void**)&box_txn_alloc);
    resolve(handle, "box_tuple_format_default", (void**)&box_tuple_format_default);
    resolve(handle, "box_tuple_new", (void**)&box_tuple_new);
    resolve(handle, "box_tuple_ref", (void**)&box_tuple_ref);
    resolve(handle, "box_tuple_unref", (void**)&box_tuple_unref);
    resolve(handle, "box_tuple_field_count", (void**)&box_tuple_field_count);
    resolve(handle, "box_tuple_bsize", (void**)&box_tuple_bsize);
    resolve(handle, "box_tuple_to_buf", (void**)&box_tuple_to_buf);
    resolve(handle, "box_tuple_format", (void**)&box_tuple_format);
    resolve(handle, "box_tuple_field", (void**)&box_tuple_field);
    resolve(handle, "box_tuple_iterator", (void**)&box_tuple_iterator);
    resolve(handle, "box_tuple_iterator_free", (void**)&box_tuple_iterator_free);
    resolve(handle, "box_tuple_position", (void**)&box_tuple_position);
    resolve(handle, "box_tuple_rewind", (void**)&box_tuple_rewind);
    resolve(handle, "box_tuple_seek", (void**)&box_tuple_seek);
    resolve(handle, "box_tuple_next", (void**)&box_tuple_next);
    resolve(handle, "box_tuple_update", (void**)&box_tuple_update);
    resolve(handle, "box_tuple_upsert", (void**)&box_tuple_upsert);
    resolve(handle, "box_tuple_extract_key", (void**)&box_tuple_extract_key);
    resolve(handle, "box_return_tuple", (void**)&box_return_tuple);
    resolve(handle, "box_space_id_by_name", (void**)&box_space_id_by_name);
    resolve(handle, "box_index_id_by_name", (void**)&box_index_id_by_name);
    resolve(handle, "box_insert", (void**)&box_insert);
    resolve(handle, "box_replace", (void**)&box_replace);
    resolve(handle, "box_delete", (void**)&box_delete);
    resolve(handle, "box_update", (void**)&box_update);
    resolve(handle, "box_upsert", (void**)&box_upsert);
    resolve(handle, "box_truncate", (void**)&box_truncate);
    resolve(handle, "box_index_iterator", (void**)&box_index_iterator);
    resolve(handle, "box_iterator_next", (void**)&box_iterator_next);
    resolve(handle, "box_iterator_free", (void**)&box_iterator_free);
    resolve(handle, "box_index_len", (void**)&box_index_len);
    resolve(handle, "box_index_bsize", (void**)&box_index_bsize);
    resolve(handle, "box_index_random", (void**)&box_index_random);
    resolve(handle, "box_index_get", (void**)&box_index_get);
    resolve(handle, "box_index_min", (void**)&box_index_min);
    resolve(handle, "box_index_max", (void**)&box_index_max);
    resolve(handle, "box_index_count", (void**)&box_index_count);
    resolve(handle, "box_error_type", (void**)&box_error_type);
    resolve(handle, "box_error_code", (void**)&box_error_code);
    resolve(handle, "box_error_message", (void**)&box_error_message);
    resolve(handle, "box_error_last", (void**)&box_error_last);
    resolve(handle, "box_error_clear", (void**)&box_error_clear);
    resolve(handle, "box_latch_new", (void**)&box_latch_new);
    resolve(handle, "box_latch_delete", (void**)&box_latch_delete);
    resolve(handle, "box_latch_lock", (void**)&box_latch_lock);
    resolve(handle, "box_latch_trylock", (void**)&box_latch_trylock);
    resolve(handle, "box_latch_unlock", (void**)&box_latch_unlock);
    resolve(handle, "clock_realtime", (void**)&clock_realtime);
    resolve(handle, "clock_monotonic", (void**)&clock_monotonic);
    resolve(handle, "clock_process", (void**)&clock_process);
    resolve(handle, "clock_thread", (void**)&clock_thread);
    resolve(handle, "clock_realtime64", (void**)&clock_realtime64);
    resolve(handle, "clock_monotonic64", (void**)&clock_monotonic64);
    resolve(handle, "clock_process64", (void**)&clock_process64);
    resolve(handle, "clock_thread64", (void**)&clock_thread64);

    dlclose(handle);
}

int fiber_invoke(va_list ap) {
    void *ctx = va_arg(ap, void*);
    void (*closure)(void*) = va_arg(ap, void*);
    closure(ctx);
    return 0;
}

void fiber_wrapper(void* ctx, void (*closure)(void*)) {
    struct fiber *swift_closure = fiber_new("fiber_wrapper", fiber_invoke);
    fiber_start(swift_closure, ctx, closure);
}

void say_wrapper(int level, const char* file, int line, const char* message) {
    int len = strlen(message);
    if (len <= 0)
        return;

    char buf[len*2];
    bzero(buf, len*2);
    for(int i = 0, j = 0; i < len; i++, j++) {
        buf[j] = message[i];
        if(buf[j] == '%') {
            buf[++j] = '%';
        }
    }

    sayfunc(level, file, line, NULL, buf, NULL);
}
