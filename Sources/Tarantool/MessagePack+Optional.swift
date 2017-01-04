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

extension Sequence where Iterator.Element == MessagePack {
    public init?(_ optional: MessagePack?) {
        guard case let .some(some) = optional,
            let value = Self(some) else {
            return nil
        }

        self = value
    }
}

extension Sequence where Iterator.Element == (key: MessagePack, value: MessagePack) {
    public init?(_ optional: MessagePack?) {
        guard case let .some(some) = optional,
            let value = Self(some) else {
            return nil
        }

        self = value
    }
}
