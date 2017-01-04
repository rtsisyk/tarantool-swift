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
import Tarantool

public struct IProtoDataSource: DataSource {
    let connection: IProtoConnection

    public init(connection: IProtoConnection) {
        self.connection = connection
    }

    public func select(spaceId: Int, iterator: Iterator = .eq, keys: Tuple = [], indexId: Int = 0, offset: Int = 0, limit: Int = 1000) throws -> [Tuple] {
        let result = try connection.request(code: .select, keys: [
            .spaceId:  .int(spaceId),
            .indexId:  .int(indexId),
            .limit:    .int(limit),
            .offset:   .int(offset),
            .iterator: .int(iterator.rawValue),
            .key:      .array(keys)]
        )

        var rows: [Tuple] = []
        for tuple in Tuple(result) {
            rows.append(Tuple(tuple)!)
        }
        return rows
    }

    public func get(spaceId: Int, keys: Tuple, indexId: Int) throws -> Tuple? {
        let result = try connection.request(code: .select, keys: [
            .spaceId:  .int(spaceId),
            .indexId:  .int(indexId),
            .limit:    .int(1),
            .offset:   .int(0),
            .iterator: .int(Iterator.eq.rawValue),
            .key:      .array(keys)]
        )

        return Tuple(Tuple(result).first)
    }

    public func insert(spaceId: Int, tuple: Tuple) throws {
        _ = try connection.request(code: .insert, keys: [
            .spaceId: .int(spaceId),
            .tuple: .array(tuple)]
        )
    }

    public func replace(spaceId: Int, tuple: Tuple) throws {
        _ = try connection.request(code: .replace, keys: [
            .spaceId: .int(spaceId),
            .tuple: .array(tuple)]
        )
    }

    public func delete(spaceId: Int, keys: Tuple, indexId: Int = 0) throws {
        _ = try connection.request(code: .delete, keys: [
            .spaceId: .int(spaceId),
            .indexId: .int(indexId),
            .key: .array(keys)]
        )
    }

    public func update(spaceId: Int, keys: Tuple, ops: Tuple, indexId: Int = 0) throws {
        _ = try connection.request(code: .update, keys: [
            .spaceId: .int(spaceId),
            .indexId: .int(indexId),
            .key:     .array(keys),
            .ops:     .array(ops)]
        )
    }

    public func upsert(spaceId: Int, tuple: Tuple, ops: Tuple, indexId: Int = 0) throws {
        _ = try connection.request(code: .upsert, keys: [
            .spaceId: .int(spaceId),
            .indexId: .int(indexId),
            .tuple:   .array(tuple),
            .ops:     .array(ops)]
        )
    }
}
