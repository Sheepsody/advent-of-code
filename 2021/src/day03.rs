#[aoc(day03, part1)]
pub fn part1(input: &str) -> u32 {
    let (mut gamma, mut epsilon) = (0, 0);
    let nbits = input.lines().next().unwrap().len();
    let nlines = input.lines().count() as u32;

    for i in 0..nbits {
        let nones: u32 = input
            .lines()
            .map(|line| line.chars().nth(i).unwrap().to_digit(2).unwrap())
            .sum();

        if 2 * nones >= nlines {
            gamma = (gamma << 1) + 1;
            epsilon = epsilon << 1;
        } else {
            gamma = gamma << 1;
            epsilon = (epsilon << 1) + 1;
        }
    }

    epsilon * gamma
}

fn get_rating(input: &str, is_most_common: bool) -> u32 {
    let nbits = input.lines().next().unwrap().len();

    let mut lines = input.lines().collect::<Vec<&str>>();

    for i in 0..nbits {
        let nones: u32 = lines
            .iter()
            .map(|line| line.chars().nth(i).unwrap().to_digit(2).unwrap())
            .sum();

        let filter_value = if (2 * nones >= (lines.len() as u32)) ^ is_most_common {
            0
        } else {
            1
        };

        lines.retain(|&line| line.chars().nth(i).unwrap().to_digit(2).unwrap() == filter_value);

        if lines.len() == 1 {
            return u32::from_str_radix(&lines[0], 2).unwrap();
        }
    }

    panic!("No valid rating found");
}

#[aoc(day03, part2)]
pub fn part2(input: &str) -> u32 {
    get_rating(input, true) * get_rating(input, false)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "00100
            11110
            10110
            10111
            10101
            01111
            00111
            11100
            10000
            11001
            00010
            01010"
        );

        assert_eq!(part1(sample), 198);
        assert_eq!(part2(sample), 230);
    }
}
