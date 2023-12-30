const SECRET: &[u8] = &[
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
];
const PRIME32_1: u32 = 0x9E3779B1;
const PRIME64_1: u64 = 0x9E3779B185EBCA87;
const PRIME64_2: u64 = 0xC2B2AE3D27D4EB4F;
const PRIME64_3: u64 = 0x165667B19E3779F9;
const XXPH3_MIDSIZE_MAX: usize = 240;
const STRIPE_LEN: usize = 64;
const ACC_NB: usize = STRIPE_LEN / std::mem::size_of::<u64>();
/// Number of secret bytes consumed at each accumulation
const XXPH_SECRET_CONSUME_RATE: usize = 8;

#[cxx::bridge(namespace = "xxph")]
mod ffi {
    extern "Rust" {
        fn xxph3_avalanche(h: u64) -> u64;
        fn xxph3_mul128_fold64(lhs: u64, rhs: u64) -> u64;
        fn xxph_read_le64(data: &[u8]) -> u64;
        fn xxph3_len_0to16(data: &[u8], seed: u64) -> u64;
        fn xxph3_len_1to3(data: &[u8], seed: u64) -> u64;
        fn xxph3_len_4to8(data: &[u8], seed: u64) -> u64;
        fn xxph3_len_9to16(data: &[u8], seed: u64) -> u64;
        fn xxph_read_le32(data: &[u8]) -> u32;
        fn xxph3_mix128b(input: &[u8], secret: &[u8], seed: u64) -> u64;
        fn xxph3_len_17to128(data: &[u8], seed: u64) -> u64;
        fn xxph3_len_129to240(data: &[u8], seed: u64) -> u64;
        fn xxph3_accumulate_512(acc: &mut [u64], input: &[u8], secret: &[u8]);
        fn xxph3_accumulate(acc: &mut [u64], input: &[u8], secret: &[u8], nb_stripes: usize);
        fn xxph3_scramble_acc(acc: &mut [u64], secret: &[u8]);
    }
}

fn xxph3_len_0to16(data: &[u8], seed: u64) -> u64 {
    assert!(data.len() <= 16);

    if data.len() > 8 {
        return xxph3_len_9to16(data, seed);
    }

    if data.len() >= 4 {
        return xxph3_len_4to8(data, seed);
    }

    if data.len() > 0 {
        return xxph3_len_1to3(data, seed);
    }

    xxph3_mul128_fold64(seed.wrapping_add(xxph_read_le64(SECRET)), PRIME64_2)
}

fn xxph3_len_1to3(data: &[u8], seed: u64) -> u64 {
    assert!(1 <= data.len() && data.len() <= 3);
    let c1 = data[0];
    let c2 = data[data.len() >> 1];
    let c3 = data[data.len() - 1];
    let combined =
        c1 as u32 | ((c2 as u32) << 8) | ((c3 as u32) << 16) | ((data.len() as u32) << 24);
    let keyed = (combined as u64) ^ (xxph_read_le32(SECRET) as u64).wrapping_add(seed);
    let mixed = keyed.wrapping_mul(PRIME64_1);
    xxph3_avalanche(mixed)
}

fn xxph3_len_4to8(data: &[u8], seed: u64) -> u64 {
    assert!(4 <= data.len() && data.len() <= 8);

    let input_lo = xxph_read_le32(data);
    let input_hi = xxph_read_le32(&data[data.len() - 4..]);
    let input_64 = input_lo as u64 ^ ((input_hi as u64) << 32);
    let keyed = input_64 ^ xxph_read_le64(SECRET).wrapping_add(seed);
    let mix64 =
        (data.len() as u64).wrapping_add((keyed ^ (keyed >> 51)).wrapping_mul(PRIME32_1 as u64));
    xxph3_avalanche((mix64 ^ (mix64 >> 47)).wrapping_mul(PRIME64_2))
}

fn xxph3_len_9to16(data: &[u8], seed: u64) -> u64 {
    assert!(9 <= data.len() && data.len() <= 16);

    let input_lo = xxph_read_le64(data) ^ (xxph_read_le64(SECRET).wrapping_add(seed));
    let input_hi = xxph_read_le64(&data[data.len() - 8..]) ^ (xxph_read_le64(&SECRET[8..]));
    let acc = (data.len() as u64)
        .wrapping_add(input_lo)
        .wrapping_add(input_hi)
        .wrapping_add(xxph3_mul128_fold64(input_lo, input_hi));
    xxph3_avalanche(acc)
}

fn xxph3_len_17to128(data: &[u8], seed: u64) -> u64 {
    assert!(17 <= data.len() && data.len() <= 128);

    let mut acc = (data.len() as u64).wrapping_mul(PRIME64_1);
    if data.len() > 32 {
        if data.len() > 64 {
            if data.len() > 96 {
                acc = acc.wrapping_add(xxph3_mix128b(&data[48..], &SECRET[96..], seed));
                acc = acc.wrapping_add(xxph3_mix128b(
                    &data[data.len() - 64..],
                    &SECRET[112..],
                    seed,
                ))
            }
            acc = acc.wrapping_add(xxph3_mix128b(&data[32..], &SECRET[64..], seed));
            acc = acc.wrapping_add(xxph3_mix128b(&data[data.len() - 48..], &SECRET[80..], seed));
        }
        acc = acc.wrapping_add(xxph3_mix128b(&data[16..], &SECRET[32..], seed));
        acc = acc.wrapping_add(xxph3_mix128b(&data[data.len() - 32..], &SECRET[48..], seed));
    }
    acc = acc.wrapping_add(xxph3_mix128b(data, SECRET, seed));
    acc = acc.wrapping_add(xxph3_mix128b(&data[data.len() - 16..], &SECRET[16..], seed));
    xxph3_avalanche(acc)
}

fn xxph3_len_129to240(data: &[u8], seed: u64) -> u64 {
    assert!(129 <= data.len() && data.len() <= XXPH3_MIDSIZE_MAX);
    let midsize_start_offset = 3;
    let midsize_last_offset = 17;

    let mut acc = (data.len() as u64).wrapping_mul(PRIME64_1);
    let nb_rounds = data.len() / 16;
    for i in 0..8 {
        acc = acc.wrapping_add(xxph3_mix128b(&data[16 * i..], &SECRET[16 * i..], seed));
    }
    acc = xxph3_avalanche(acc);
    assert!(nb_rounds >= 8);
    for i in 8..nb_rounds {
        acc = acc.wrapping_add(xxph3_mix128b(
            &data[16 * i..],
            &SECRET[16 * (i - 8) + midsize_start_offset..],
            seed,
        ));
    }
    // last bytes
    let secret_size_min = 136;

    acc = acc.wrapping_add(xxph3_mix128b(
        &data[data.len() - 16..],
        &SECRET[secret_size_min - midsize_last_offset..],
        seed,
    ));
    xxph3_avalanche(acc)
}

fn xxph_read_le64(data: &[u8]) -> u64 {
    u64::from_le_bytes(data[..std::mem::size_of::<u64>()].try_into().unwrap())
}

fn xxph_read_le32(data: &[u8]) -> u32 {
    u32::from_le_bytes(data[..std::mem::size_of::<u32>()].try_into().unwrap())
}

fn xxph3_mul128_fold64(lhs: u64, rhs: u64) -> u64 {
    let product = (lhs as u128).wrapping_mul(rhs as u128);
    let low = product & 0xFFFF_FFFF_FFFF_FFFF;
    let high = product >> 64;
    (low ^ high) as u64
}

fn xxph3_avalanche(mut h: u64) -> u64 {
    h = h ^ (h >> 37);
    h = h.wrapping_mul(PRIME64_3);
    h = h ^ (h >> 32);
    h
}

fn xxph3_mix128b(input: &[u8], secret: &[u8], seed: u64) -> u64 {
    let input_lo = xxph_read_le64(input);
    let input_hi = xxph_read_le64(&input[8..]);
    xxph3_mul128_fold64(
        input_lo ^ xxph_read_le64(secret).wrapping_add(seed),
        input_hi ^ xxph_read_le64(&secret[8..]).wrapping_sub(seed),
    )
}

fn xxph3_accumulate_512(acc: &mut [u64], input: &[u8], secret: &[u8]) {
    // TODO: use SIMD for SSE2 and AVX2
    for i in 0..ACC_NB {
        let data_val = xxph_read_le64(&input[8 * i..]);
        let data_key = data_val ^ xxph_read_le64(&secret[8 * i..]);

        acc[i] = acc[i].wrapping_add(data_val);
        acc[i] = acc[i].wrapping_add((data_key & 0xFFFF_FFFF).wrapping_mul(data_key >> 32));
    }
}

fn xxph3_accumulate(acc: &mut [u64], input: &[u8], secret: &[u8], nb_stripes: usize) {
    for n in 0..nb_stripes {
        xxph3_accumulate_512(
            acc,
            &input[n * STRIPE_LEN..],
            &secret[n * XXPH_SECRET_CONSUME_RATE..],
        )
    }
}

fn xxph3_scramble_acc(acc: &mut [u64], secret: &[u8]) {
    // TODO: use SIMD
    for i in 0..ACC_NB {
        let key64 = xxph_read_le64(&secret[8 * i..]);
        let mut acc64 = acc[i];
        acc64 = acc64 ^ (acc64 >> 47);
        acc64 = acc64 ^ key64;
        acc64 = acc64.wrapping_mul(PRIME32_1 as u64);
        acc[i] = acc64;
    }
}
