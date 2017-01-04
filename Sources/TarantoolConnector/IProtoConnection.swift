/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

import Socket
import Foundation

@_exported import Tarantool
@_exported import MessagePack

public typealias Keys = [Key : MessagePack]

public class IProtoConnection {
    let socket: Socket
    let welcome: Welcome

    public init(host: String, port: UInt16 = 3301, awaiter: IOAwaiter? = nil) throws {
        socket = try Socket(awaiter: awaiter)
        try socket.connect(to: host, port: port)

        welcome = Welcome()
        guard try socket.read(to: &welcome.buffer) == welcome.buffer.count else {
            throw IProtoError.invalidWelcome(reason: .invalidSize)
        }

        guard welcome.isValid else {
            throw IProtoError.invalidWelcome(reason: .invalidHeader)
        }
    }

    deinit {
        try? socket.close(silent: true)
    }

    private func send(code: Code, keys: Keys = [:], sync: MessagePack? = nil, schemaId: MessagePack? = nil) throws {
        // 2/3 - header - MP_MAP
        var header: Map = [:]
        header[Key.code.rawValue] = code.rawValue
        if let sync = sync {
            header[Key.sync.rawValue] = sync
        }
        if let schemaId = schemaId {
            header[Key.schemaId.rawValue] = schemaId
        }

        // 3/3 - body - MP_MAP
        var body: Map = [:]
        for (key, value) in keys {
            body[key.rawValue] = value
        }

        var serializer = MPSerializer()
        serializer.pack(header)
        serializer.pack(body)
        let packet = serializer.bytes

        // 1/3 - header + body size
        let size = try HeaderLength(packet.count).bytes

        _ = try socket.write(bytes: size + packet)
    }

    private func receive() throws -> (header: MessagePack, body: MessagePack) {
        let length = try readPacketLength()

        var buffer = [UInt8](repeating: 0, count: length)
        guard try socket.read(to: &buffer) == length else {
            throw IProtoError.invalidPacket(reason: .invalidSize)
        }

        var deserializer = MPDeserializer(bytes: buffer)
        let header = try deserializer.unpack() as MessagePack
        let body = try deserializer.unpack() as MessagePack

        return (header, body)
    }

    private func readPacketLength() throws -> Int {
        // always packed as 32bit integer CE XX XX XX XX
        var lengthBuffer = [UInt8](repeating: 0, count: 5)
        guard try socket.read(to: &lengthBuffer) == 5 else {
            throw IProtoError.invalidPacket(reason: .invalidSize)
        }
        return try HeaderLength(bytes: lengthBuffer).length
    }

    public func request(code: Code, keys: Keys = [:], sync: MessagePack? = nil, schemaId: MessagePack? = nil) throws -> Tuple {
        try send(code: code, keys: keys, sync: sync, schemaId: schemaId)
        let (header, body) = try receive()

        //check header
        guard let packedErrorCode = Map(header)?[0],
            let errorCode = Int(packedErrorCode) else {
            throw IProtoError.invalidPacket(reason: .invalidHeader)
        }
        guard errorCode < 0x8000 else {
            //error packed as [0x31 : MP_STRING]
            guard let errorMessage = body.map?[Key.error.rawValue], let description = String(errorMessage) else {
                throw IProtoError.badRequest(code: errorCode, message: "nil")
            }
            throw IProtoError.badRequest(code: errorCode, message: description)
        }


        guard let response = Map(body) else {
            throw IProtoError.invalidPacket(reason: .invalidBody)
        }

        //empty body e.g. ping response
        guard response.count != 0 else {
            return []
        }

        //body packed as [0x30 : MP_MAP]
        guard let data = response[Key.data.rawValue], let tuple = Tuple(data) else {
            throw IProtoError.invalidPacket(reason: .invalidBody)
        }
        return tuple
    }
}

extension IProtoConnection {
    @discardableResult
    public func ping() throws {
        _ = try request(code: .ping)
    }

    public func call(_ function: String, with tuple: Tuple = []) throws -> Tuple {
        return try request(code: .call, keys: [.functionName: .string(function), .tuple: .array(tuple)])
    }

    public func eval(_ expression: String, with tuple: Tuple = []) throws -> Tuple {
        return try request(code: .call, keys: [.functionName: .string(expression), .tuple: .array(tuple)])
    }

    public func auth(username: String, password: String) throws {
        let data = [UInt8](password.utf8)
        guard let salt = Data(base64Encoded: welcome.salt) else {
            throw IProtoError.invalidSalt
        }

        let scramble = data.chapSha1(salt: [UInt8](salt))

        let keys: Keys = [
            .username: .string(username),
            .tuple: .array([.string("chap-sha1"), .binary(scramble)])
        ]
        _ = try request(code: .auth, keys: keys)
    }
}
