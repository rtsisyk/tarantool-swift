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

extension Box {
    public struct Transaction {
        public enum Action {
            case commit, rollback
        }

        fileprivate static func begin() throws {
            guard box_txn_begin() == 0 else {
                throw BoxError()
            }
        }

        fileprivate static func commit() throws {
            guard box_txn_commit() == 0 else {
                throw BoxError()
            }
        }

        fileprivate static func rollback() throws {
            guard box_txn_rollback() == 0 else {
                throw BoxError()
            }
        }

        fileprivate static func run(_ closure: (Void) throws -> Action) throws {
            try begin()

            guard let action = try? closure() else {
                try rollback()
                return
            }

            switch action {
            case .commit: try commit()
            case .rollback: try rollback()
            }
        }
    }

    public static func transaction(_ closure: (Void) throws -> Box.Transaction.Action) throws {
        try Transaction.run(closure)
    }
}
