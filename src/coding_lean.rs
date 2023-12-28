pub(crate) fn encode_fixed_64(value: u64) -> [u8; 8] {
    value.to_le_bytes()
}
