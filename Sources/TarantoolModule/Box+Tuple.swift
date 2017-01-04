/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import Tarantool
import CTarantool
import MessagePack

extension Box {
    static func unpackTuple(_ tuple: OpaquePointer) throws -> Tuple {
        let size = box_tuple_bsize(tuple)
        guard size > 0 else {
            throw TarantoolError.invalidTuple(message: "tuple size: \(size)")
        }
        let packed = UnsafeMutablePointer<UInt8>.allocate(capacity: size)
        defer { packed.deallocate(capacity: size) }
        // copying internal tuple buffer
        let written = packed.withMemoryRebound(to: CChar.self, capacity: size) { pointer in
            return box_tuple_to_buf(tuple, pointer, size)
        }

        let unpacked = try MessagePack.deserialize(bytes: UnsafeBufferPointer(start: packed, count: written))
        guard let tuple = Tuple(unpacked) else {
            throw TarantoolError.invalidTuple(message: "array result was expected")
        }

        return tuple
    }

    public static func returnTuple(_ tuple: Tuple, to context: OpaquePointer) -> Int32 {
        let bytes = MessagePack.serialize(.array(tuple))
        let pointer = UnsafeRawPointer(bytes).assumingMemoryBound(to: Int8.self)
        let tuple = box_tuple_new(box_tuple_format_default(), pointer, pointer+bytes.count)
        return box_return_tuple(context, tuple)
    }
}
