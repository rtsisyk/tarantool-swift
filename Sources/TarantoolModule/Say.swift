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

public struct Say {
    enum Level: Int32 {
        case fatal, systemError, error, critical, warning, info, debug
    }

    public static func fatal(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.fatal.rawValue, file, line, message)
    }

    public static func systemError(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.systemError.rawValue, file, line, message)
    }

    public static func error(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.error.rawValue, file, line, message)
    }

    public static func critical(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.critical.rawValue, file, line, message)
    }

    public static func warning(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.warning.rawValue, file, line, message)
    }

    public static func info(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.info.rawValue, file, line, message)
    }

    public static func debug(message: String, file: String = #file, line: Int32 = #line) {
        say_wrapper(Level.debug.rawValue, file, line, message)
    }
}
