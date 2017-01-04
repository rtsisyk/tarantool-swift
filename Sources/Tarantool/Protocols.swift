/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import MessagePack

public typealias Tuple = [MessagePack]
public typealias Map = [MessagePack : MessagePack]

public protocol SchemaProtocol {
    var spaces: [String : Space] { get }
}

public protocol DataSource {
    func select(spaceId: Int, iterator: Iterator, keys: Tuple, indexId: Int, offset: Int, limit: Int) throws -> [Tuple]
    func get(spaceId: Int, keys: Tuple, indexId: Int) throws -> Tuple?
    func insert(spaceId: Int, tuple: Tuple) throws
    func replace(spaceId: Int, tuple: Tuple) throws
    func delete(spaceId: Int, keys: Tuple, indexId: Int) throws
    func update(spaceId: Int, keys: Tuple, ops: Tuple, indexId: Int) throws
    func upsert(spaceId: Int, tuple: Tuple, ops: Tuple, indexId: Int) throws
}
