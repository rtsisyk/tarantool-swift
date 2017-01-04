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

extension Box {
    static func getSpaceIdByName(_ name: [UInt8]) throws -> UInt32 {
        let name = unsafeBitCast(name, to: [CChar].self)
        let id = box_space_id_by_name(name, UInt32(name.count))
        if id == UInt32(Int32.max) {
            throw TarantoolError.spaceNotFound
        }
        return id
    }

    static func getIndexIdByName(_ name: [UInt8], spaceId: UInt32) throws -> UInt32 {
        let name = unsafeBitCast(name, to: [CChar].self)
        let id = box_index_id_by_name(spaceId, name, UInt32(name.count))
        if id == UInt32(Int32.max) {
            throw TarantoolError.indexNotFound
        }
        return id
    }

    static func copyToInternalMemory(_ bytes: [UInt8]) throws -> UnsafePointer<CChar> {
        // will be deallocated on the fiber death
        guard let buffer = box_txn_alloc(bytes.count) else {
            throw TarantoolError.notEnoughMemory
        }
        memcpy(buffer, bytes, bytes.count)
        return UnsafeRawPointer(buffer).assumingMemoryBound(to: CChar.self)
    }
}
