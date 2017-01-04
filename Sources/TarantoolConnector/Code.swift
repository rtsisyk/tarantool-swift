/*
 * Copyright 2017 Tris Foundation and the project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 *
 * See LICENSE.txt in the project root for license information
 * See CONTRIBUTORS.txt for the list of the project authors
 */

public enum Code: MessagePack {
    case select    = 0x01
    case insert    = 0x02
    case replace   = 0x03
    case update    = 0x04
    case delete    = 0x05
    case auth      = 0x07
    case eval      = 0x08
    case upsert    = 0x09
    case call      = 0x0A

    case ping      = 0x40
    case join      = 0x41
    case subscribe = 0x42
}
