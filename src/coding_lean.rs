pub(crate) fn encode_fixed_64(value: u64) -> [u8; 8] {
    value.to_le_bytes()
}

pub(crate) fn decode_fixed_64(bytes: &[u8]) -> u64 {
    u64::from_le_bytes(bytes.try_into().unwrap())
}
