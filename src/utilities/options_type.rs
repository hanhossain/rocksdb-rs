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
}
