/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import Async
import CTarantool
import TarantoolModule

public struct TarantoolLoop: AsyncLoop {
    public func run() {
        // fallback to tarantool built-in evloop
    }
}

public struct AsyncTarantool: Async {
    public init() {}
    public var loop: AsyncLoop = TarantoolLoop()
    public var task: (@escaping AsyncTask) -> Void = fiber
    public var awaiter: IOAwaiter? = TarantoolAwaiter()
}

public struct TarantoolAwaiterTimeout: Error {

}

public struct TarantoolAwaiter: IOAwaiter {
    public init() {}

    struct Timeout {
        static let infinity: Double = 100*365*24*3600
    }

    struct COIOEvent {
        static let read: Int32 = 0x1
        static let write: Int32 = 0x2
    }

    public func wait(for descriptor: Int32, event: IOEvent) throws {
        switch event {
        case .read:
            guard COIOEvent.read == coio_wait(descriptor, COIOEvent.read, Timeout.infinity) else {
                throw TarantoolAwaiterTimeout()
            }
        case .write:
            guard COIOEvent.write == coio_wait(descriptor, COIOEvent.write, Timeout.infinity) else {
                throw TarantoolAwaiterTimeout()
            }
        }
    }
}
