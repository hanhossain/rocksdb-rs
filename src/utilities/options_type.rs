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

    /// A set of modifier flags used to alter how an option is evaluated or
    /// processed. These flags can be combined together (e.g. kMutable | kShared).
    /// The kCompare flags can be used to control if/when options are compared.
    /// If kCompareNever is set, two related options would never be compared (always
    /// equal) If kCompareExact is set, the options will only be compared if the
    /// sanity mode
    ///                  is exact
    /// kMutable       means the option can be changed after it is prepared
    /// kShared        means the option is contained in a std::shared_ptr
    /// kUnique        means the option is contained in a std::uniqued_ptr
    /// kRawPointer    means the option is a raw pointer value.
    /// kAllowNull     means that an option is allowed to be null for verification
    ///                purposes.
    /// kDontSerialize means this option should not be serialized and included in
    ///                the string representation.
    /// kDontPrepare   means do not call PrepareOptions for this pointer value.
    enum OptionTypeFlags {
        /// No flags
        kNone = 0x00,
        kCompareNever = 0x01,
        kCompareLoose = 0x02,
        kCompareExact = 0xFF,

        /// Option is mutable
        kMutable = 0x0100,
        /// The option is stored as a raw pointer
        kRawPointer = 0x0200,
        /// The option is stored as a shared_ptr
        kShared = 0x0400,
        /// The option is stored as a unique_ptr
        kUnique = 0x0800,
        /// The option can be null
        kAllowNull = 0x1000,
        /// Don't serialize the option
        kDontSerialize = 0x2000,
        /// Don't prepare or sanitize this option
        kDontPrepare = 0x4000,
        /// The option serializes to a name only
        kStringNameOnly = 0x8000,
    }
}
