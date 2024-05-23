#[cxx::bridge(namespace = "rocksdb_rs::utilities::options_type")]
mod ffix {
    /// The underlying "class/type" of the option.
    /// This enum is used to determine how the option should
    /// be converted to/from strings and compared.
    enum OptionType {
        kBoolean,
        kInt,
        kInt32T,
        kInt64T,
        kUInt,
        kUInt8T,
        kUInt32T,
        kUInt64T,
        kSizeT,
        kString,
        kDouble,
        kCompactionStyle,
        kCompactionPri,
        kCompressionType,
        kCompactionStopStyle,
        kChecksumType,
        kEncodingType,
        kEnv,
        kEnum,
        kStruct,
        kVector,
        kConfigurable,
        kCustomizable,
        kEncodedString,
        kTemperature,
        kArray,
        kUnknown,
    }

    enum OptionVerificationType {
        kNormal,
        /// The option is pointer typed so we can only verify
        /// based on it's name.
        kByName,
        /// Same as kByName, but it also allows the case
        /// where one of them is a nullptr.
        kByNameAllowNull,
        /// Same as kByName, but it also allows the case
        /// where the old option is nullptr.
        kByNameAllowFromNull,
        /// The option is no longer used in rocksdb. The RocksDB
        /// OptionsParser will still accept this option if it
        /// happen to exists in some Options file.  However,
        /// the parser will not include it in serialization
        /// and verification processes.
        kDeprecated,
        /// This option represents is a name/shortcut for
        /// another option and should not be written or verified
        /// independently
        kAlias,
    }
}
