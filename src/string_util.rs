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

pub(crate) fn consume_decimal_number(s: &mut String) -> Option<u64> {
    let re_match = regex::Regex::new(r"^\d+").unwrap().find(s)?;
    s.drain(..re_match.end()).as_str().parse::<u64>().ok()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_consume_decimal_number_valid_number() {
        let mut s = "1234something".to_string();
        assert_eq!(consume_decimal_number(&mut s), Some(1234));
        assert_eq!(s, "something");
    }

    #[test]
    fn consume_decimal_number_leading_zeros() {
        let mut s = "0001234something".to_string();
        assert_eq!(consume_decimal_number(&mut s), Some(1234));
        assert_eq!(s, "something");
    }

    #[test]
    fn consume_decimal_number_zeros() {
        let mut s = "00000something".to_string();
        assert_eq!(consume_decimal_number(&mut s), Some(0));
        assert_eq!(s, "something");
    }

    #[test]
    fn consume_decimal_number_just_number() {
        let mut s = "5678".to_string();
        assert_eq!(consume_decimal_number(&mut s), Some(5678));
        assert!(s.is_empty());
    }

    #[test]
    fn consume_decimal_number_no_number() {
        let mut s = "something".to_string();
        assert_eq!(consume_decimal_number(&mut s), None);
        assert_eq!(s, "something");
    }

    #[test]
    fn consume_decimal_number_overflow() {
        let mut s = "18446744073709551616".to_string();
        assert_eq!(consume_decimal_number(&mut s), None);
    }
}
