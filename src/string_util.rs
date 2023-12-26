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
