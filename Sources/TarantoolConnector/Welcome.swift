/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

fileprivate let headerSize = 64
fileprivate let saltSize = 44
fileprivate let size = 128

struct Welcome {
    var buffer = [UInt8](repeating: 0, count: size)

    var header: String {
        return String(slice: buffer.prefix(upTo: headerSize))
    }

    var salt: String {
        return String(slice: buffer[headerSize..<headerSize+saltSize])
    }

    var isValid: Bool {
        return header.hasPrefix("Tarantool")
    }
}

fileprivate extension String {
    init(slice: ArraySlice<UInt8>) {
        self = String(cString: Array(slice) + [0])
    }
}
