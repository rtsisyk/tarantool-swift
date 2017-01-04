/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import CryptoSwift

// Use Array where Element == UInt8 when swift 3.1 is out
extension Sequence where Iterator.Element == UInt8 {
    func chapSha1(salt: [UInt8]) -> [UInt8] {
        let scrambleSize = 20
        let data: [UInt8] = self as! [UInt8]

        var step1 = [UInt8](repeating: 0, count: scrambleSize)
        var step2 = [UInt8](repeating: 0, count: scrambleSize)
        var step3 = [UInt8](repeating: 0, count: scrambleSize)
        var scramble = [UInt8](repeating: 0, count: scrambleSize)

        step1 = data.sha1()
        step2 = step1.sha1()
        step3 = (salt.prefix(upTo: scrambleSize) + step2).sha1()

        for i in 0 ..< scrambleSize {
            scramble[i] = step1[i] ^ step3[i]
        }
        return scramble
    }
}
