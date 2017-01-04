/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

/***********************************************
 * Description taken from Tarantool source code
 ***********************************************/

/*
 * Controls how to iterate over tuples in an index.
 * Different index types support different iterator types.
 * For example, one can start iteration from a particular value
 * (request key) and then retrieve all tuples where keys are
 * greater or equal (= .ge) to this key.
 *
 * If iterator type is not supported by the selected index type,
 * iterator constructor must fail with ER_UNSUPPORTED. To be
 * selectable for primary key, an index must support at least
 * .eq and .ge types.
 *
 * NULL value of request key corresponds to the first or last
 * key in the index, depending on iteration direction.
 * (first key for .ge and .gt types, and last key for .le and .lt).
 * Therefore, to iterate over all tuples in an index, one can
 * use .ge or .le iteration types with start key equal
 * to NULL.
 * For .eq, the key must not be NULL.
 */

public enum Iterator: Int {
    /* EQ must be the first member for request_create  */
    case eq                =  0 /* key == x ASC order                  */
    case req               =  1 /* key == x DESC order                 */
    case all               =  2 /* all tuples                          */
    case lt                =  3 /* key <  x                            */
    case le                =  4 /* key <= x                            */
    case ge                =  5 /* key >= x                            */
    case gt                =  6 /* key >  x                            */
    case bitsAllSet        =  7 /* all bits from x are set in key      */
    case bitsAnySet        =  8 /* at least one x's bit is set         */
    case bitsAllNotSet     =  9 /* all bits are not set                */
    case overlaps          = 10 /* key overlaps x                      */
    case neighbor          = 11 /* typles in distance ascending order from specified point */
    case iteratorTypeMAX
};
