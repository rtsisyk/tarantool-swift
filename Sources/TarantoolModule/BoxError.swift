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

public struct BoxError: Error {
    public let code: Int
    public let message: String

    init(){
        guard let errorPointer = box_error_last() else {
            self.code = 0
            self.message = "success"
            return
        }
        self.code = Int(box_error_code(errorPointer))
        self.message  = String(cString: box_error_message(errorPointer)!)
    }
}

extension BoxError: CustomStringConvertible {
    public var description: String {
        return "code: \(code) message: \(message)"
    }
}
