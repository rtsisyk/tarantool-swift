/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

struct HeaderLength {
    let length: Int

    var bytes: [UInt8] {
        var bytes = [UInt8](repeating: 0, count: 5)
        bytes[0] = 0xce
        bytes[1] = UInt8(truncatingBitPattern: length >> 24)
        bytes[2] = UInt8(truncatingBitPattern: length >> 16)
        bytes[3] = UInt8(truncatingBitPattern: length >> 8)
        bytes[4] = UInt8(truncatingBitPattern: length)
        return bytes
    }

    init(_ length: Int) throws {
        guard length <= Int(Int32.max) else {
            throw IProtoError.invalidPacket(reason: .invalidSize)
        }
        self.length = length
    }

    init(bytes: [UInt8]) throws {
        guard bytes[0] == 0xce else {
            throw MessagePackError.invalidData
        }
        guard bytes.count >= 5 else {
            throw MessagePackError.insufficientData
        }

        self.length = Int(bytes[1]) << 24 | Int(bytes[2]) << 16 | Int(bytes[3]) << 8 | Int(bytes[4])
    }
}
