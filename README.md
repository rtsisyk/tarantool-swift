# Tarantool

This package consists of two modules for [Tarantool](https://tarantool.org) database

1. TarantoolConnector is iproto tcp connector for communicating with remote tarantool instance.
2. TarantoolModule is an interface to internal tarantool C API for writing tarantool stored procedures in swift.

## Package.swift

```swift
.Package(url: "https://github.com/tris-foundation/tarantool.git", majorVersion: 0)
```

## Usage

You can find this code and more in [examples](https://github.com/tris-foundation/examples).

### Tarantool Connector

```swift
let connection = try IProtoConnection(host: "127.0.0.1")
try connection.auth(username: "tester", password: "tester")

let source = IProtoDataSource(connection: connection)
let schema = try Schema(source)

guard let test = schema.spaces["test"] else {
    print("space test not found")
    exit(0)
}

print(try test.select(.eq, keys: [3]))
// [[3, "baz"]]

print(try test.select(.all))
// first run: [[1, "foo"], [2, "bar"], [3, "baz"]]
// next runs: [[42, "Answer to ... and Everything"], [1, "foo"], ...]

try test.replace([42, "Answer to the Ultimate Question of Life, The Universe, and Everything"])
print(try test.select(.eq, keys: [42]))
// [42, "Answer to the Ultimate Question of Life, The Universe, and Everything"]
```

### Tarantool Module

```swift
typealias BoxResult = Int32

struct ModuleError: Error {
    let message: String
}

@_silgen_name("helloSwift")
func helloSwift(context: OpaquePointer) -> BoxResult {
    return Box.returnTuple(["hello from swift"], to: context)
}

@_silgen_name("getFoo")
func getFoo(context: OpaquePointer) -> BoxResult {
    do {
        let source = BoxDataSource()
        let schema = try Schema(source)

        guard let space = schema.spaces["data"] else {
            throw ModuleError(message: "space 'data' not found")
        }

        guard let result = try space.get(["foo"]) else {
            throw ModuleError(message: "foo not found")
        }

        return Box.returnTuple(result, to: context)
    } catch {
        Say.error(message: String(describing: error))
        return -1
    }
}
```
```swift
let iproto = try IProtoConnection(host: "127.0.0.1")

print(try iproto.call("helloSwift"))
print(try iproto.call("getFoo"))
```
