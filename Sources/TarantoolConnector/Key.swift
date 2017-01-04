/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

public enum Key: MessagePack {
    case code         = 0x00
    case sync         = 0x01
    /* Replication keys (header) */
    case serverId     = 0x02
    case lsn          = 0x03
    case timestamp    = 0x04
    case schemaId     = 0x05
    /* Leave a gap for other keys in the header. */
    case spaceId      = 0x10
    case indexId      = 0x11
    case limit        = 0x12
    case offset       = 0x13
    case iterator     = 0x14
    case indexBase    = 0x15
    /* Leave a gap between integer values and other keys */
    case key          = 0x20
    case tuple        = 0x21
    case functionName = 0x22
    case username     = 0x23
    /* Replication keys (body) */
    case serverUUID   = 0x24
    case clusterUUID  = 0x25
    case vClock       = 0x26
    case expression   = 0x27 /* eval */
    case ops          = 0x28 /* UPSERT but not UPDATE ops, because of legacy */
    /* Leave a gap between request keys and response keys */
    case data         = 0x30
    case error        = 0x31
    case keyMax
}
