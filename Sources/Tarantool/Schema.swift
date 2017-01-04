/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

let _space: Int = 280
let _vspace: Int = 281

public struct Schema: SchemaProtocol {
    public let spaces: [String: Space]
    public init(_ source: DataSource) throws {
        let tuples = try source.select(spaceId: _vspace, iterator: .all, keys: [], indexId: 0, offset: 0, limit: Int.max)

        var spaces: [String: Space] = [:]
        for tuple in tuples {
            guard
                let id = Int(tuple[0]),
                let name = String(tuple[2]) else {
                    throw TarantoolError.invalidSchema
            }
            spaces[name] = Space(id: id, source: source)
        }
        self.spaces = spaces
    }
}
