/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import CTarantool
import MessagePack

@_exported import Tarantool

public struct BoxDataSource: DataSource {
    public init() {}

    public func select(spaceId: Int, iterator: Iterator, keys: Tuple = [], indexId: Int = 0, offset: Int = 0, limit: Int = Int.max) throws -> [Tuple] {
        let keys = MessagePack.serialize(.array(keys))
        return try Box.select(spaceId: UInt32(spaceId), iterator: iterator, indexId: UInt32(indexId), keys: keys)
    }

    public func get(spaceId: Int, keys: Tuple, indexId: Int = 0) throws -> Tuple? {
        let keys = MessagePack.serialize(.array(keys))
        return try Box.get(spaceId: UInt32(spaceId), indexId: UInt32(indexId), keys: keys)
    }

    public func insert(spaceId: Int, tuple: Tuple) throws {
        let tuple = MessagePack.serialize(.array(tuple))
        try Box.insert(spaceId: UInt32(spaceId), tuple: tuple)
    }

    public func replace(spaceId: Int, tuple: Tuple) throws {
        let tuple = MessagePack.serialize(.array(tuple))
        try Box.replace(spaceId: UInt32(spaceId), tuple: tuple)
    }

    public func delete(spaceId: Int, keys: Tuple, indexId: Int) throws {
        let keys = MessagePack.serialize(.array(keys))
        try Box.delete(spaceId: UInt32(spaceId), indexId: UInt32(indexId), keys: keys)
    }

    public func update(spaceId: Int, keys: Tuple, ops: Tuple, indexId: Int) throws {
        let keys = MessagePack.serialize(.array(keys))
        let ops = MessagePack.serialize(.array(ops))
        try Box.update(spaceId: UInt32(spaceId), indexId: UInt32(indexId), keys: keys, ops: ops)
    }

    public func upsert(spaceId: Int, tuple: Tuple, ops: Tuple, indexId: Int) throws {
        let tuple = MessagePack.serialize(.array(tuple))
        let ops = MessagePack.serialize(.array(ops))
        try Box.upsert(spaceId: UInt32(spaceId), indexId: UInt32(indexId), tuple: tuple, ops: ops)
    }
}
