/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import Foundation

public struct Space {
    public let id: Int
    public let source: DataSource

    public init(id: Int, source: DataSource) {
        self.source = source
        self.id = id
    }

    public func select(_ iterator: Iterator, keys: Tuple = [], indexId: Int = 0, offset: Int = 0, limit: Int = Int.max) throws -> [Tuple] {
        return try source.select(spaceId: id, iterator: iterator, keys: keys, indexId: indexId, offset: offset, limit: limit)
    }

    public func get(_ keys: Tuple, indexId: Int = 0) throws -> Tuple? {
        return try source.get(spaceId: id, keys: keys, indexId: indexId)
    }

    public func insert(_ tuple: Tuple) throws {
        try source.insert(spaceId: id, tuple: tuple)
    }

    public func replace(_ tuple: Tuple) throws {
        try source.replace(spaceId: id, tuple: tuple)
    }

    public func delete(_ keys: Tuple, indexId: Int = 0) throws {
        try source.delete(spaceId: id, keys: keys, indexId: indexId)
    }

    public func update(_ keys: Tuple, ops: Tuple = [], indexId: Int = 0) throws {
        try source.update(spaceId: id, keys: keys, ops: ops, indexId: indexId)
    }

    public func upsert(_ tuple: Tuple, ops: Tuple = [], indexId: Int = 0) throws {
        try source.upsert(spaceId: id, tuple: tuple, ops: ops, indexId: indexId)
    }
}

extension Space: CustomStringConvertible {
    public var description: String {
        return "space: \(id)"
    }
}
