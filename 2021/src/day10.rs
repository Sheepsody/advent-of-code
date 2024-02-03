const OPENING: [char; 4] = ['{', '[', '(', '<'];
const CLOSING: [char; 4] = ['}', ']', ')', '>'];

const CLOSING_SCORE: [(char, u32); 4] = [(')', 3), (']', 57), ('}', 1197), ('>', 25137)];

fn get_first_illegal(line: &str) -> Option<char> {
    let mut stack = Vec::new();

    for c in line.chars() {
        match c {
            // Opening
            o if OPENING.contains(&o) => {
                stack.push(o);
            }
            // Closing
            c if CLOSING.contains(&c) => {
                let o = stack.pop().unwrap();
                match (o, c) {
                    ('{', '}') | ('[', ']') | ('(', ')') | ('<', '>') => (),
                    _ => return Some(c),
                }
            }
            _ => unreachable!(),
        }
    }

    None
}

#[aoc(day10, part1)]
fn part1(input: &str) -> u32 {
    input
        .lines()
        .filter_map(get_first_illegal)
        .map(|c| CLOSING_SCORE.iter().find(|(x, _)| x == &c).unwrap().1)
        .sum()
}

fn get_remaining_stack(line: &str) -> Option<Vec<char>> {
    line.chars().fold(
        Some(Vec::new()),
        |stack: Option<Vec<char>>, c| match stack {
            Some(mut s) => match c {
                // Opening
                o if OPENING.contains(&o) => {
                    s.push(o);
                    Some(s)
                }
                // Closing
                c if CLOSING.contains(&c) => {
                    let o = s.pop().unwrap();
                    match (o, c) {
                        ('{', '}') | ('[', ']') | ('(', ')') | ('<', '>') => Some(s),
                        _ => None,
                    }
                }
                // Illegal
                _ => unreachable!(),
            },
            _ => None,
        },
    )
}

// Just need to match score of opposing brackets
const OPENING_SCORE: [(char, u32); 4] = [('(', 1), ('[', 2), ('{', 3), ('<', 4)];

#[aoc(day10, part2)]
fn part2(input: &str) -> u64 {
    let mut scores: Vec<u64> = input
        .lines()
        .filter_map(get_remaining_stack)
        .map(|stack| {
            stack.iter().rev().fold(0_u64, |acc, &c| {
                acc * 5 + OPENING_SCORE.iter().find(|(x, _)| x == &c).unwrap().1 as u64
            })
        })
        .collect();

    scores.sort();
    scores[scores.len() / 2]
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample_part1() {
        assert_eq!(part1("[{[{({}]{}}([{[{{{}}([]"), 57);
    }

    #[test]
    fn sample_part2() {
        assert_eq!(part2("[({(<(())[]>[[{[]{<()<>>"), 288957);
    }

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "[({(<(())[]>[[{[]{<()<>>
            [(()[<>])]({[<{<<[]>>(
            {([(<{}[<>[]}>{[]{[(<()>
            (((({<>}<{<{<>}{[]{[]{}
            [[<[([]))<([[{}[[()]]]
            [{[{({}]{}}([{[{{{}}([]
            {<[[]]>}<{[{[{[]{()[[[]
            [<(<(<(<{}))><([]([]()
            <{([([[(<>()){}]>(<<{{
            <{([{{}}[<[[[<>{}]]]>[]]"
        );

        assert_eq!(part1(sample), 26397);
        assert_eq!(part2(sample), 288957);
    }
}
