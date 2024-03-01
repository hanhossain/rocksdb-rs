const LOWERCASE_DIGITS: [char; 36] = [
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
];
const UPPERCASE_DIGITS: [char; 36] = [
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
];

pub(crate) fn put_base_chars(s: &mut String, n: usize, mut v: u64, uppercase: bool) {
    let digits = if uppercase {
        &UPPERCASE_DIGITS
    } else {
        &LOWERCASE_DIGITS
    };

    let mut stack = Vec::new();
    for _ in 0..n {
        stack.push(digits[(v % digits.len() as u64) as usize]);
        v = v / digits.len() as u64;
    }
    while let Some(c) = stack.pop() {
        s.push(c);
    }
}

// TODO: this could probably use the alphanumeric methods
pub(crate) fn parse_base_chars(s: &str, base: u64, v: &mut u64) -> bool {
    for c in s.chars() {
        *v = *v * base;
        if c >= '0' && ((base >= 10 && c <= '9') || base < 10 && c < '0') {
            *v += (c as u64) - ('0' as u64);
        } else if base > 10 && c >= 'A' && c < ('A' as u8 + base as u8 - 10) as char {
            *v += (c as u64) - 'A' as u64 + 10;
        } else if base > 10 && c >= 'a' && c < ('a' as u8 + base as u8 - 10) as char {
            *v += (c as u64) - 'a' as u64 + 10;
        } else {
            return false;
        }
    }
    true
}
