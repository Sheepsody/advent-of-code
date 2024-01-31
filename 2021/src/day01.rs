#[aoc_generator(day01)]
pub fn input_generator(input: &str) -> Vec<u32> {
    input.lines().map(|l| l.parse::<u32>().unwrap()).collect()
}

#[aoc(day01, part1)]
pub fn part1(input: &[u32]) -> i32 {
    input.windows(2).filter(|&w| w[1] > w[0]).count() as i32
}

#[aoc(day01, part2)]
pub fn part2(input: &[u32]) -> i32 {
    input
        .windows(3)
        .map(|w| w.iter().sum())
        .collect::<Vec<u32>>()
        .windows(2)
        .filter(|&w| w[1] > w[0])
        .count() as i32
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sample1() {
        let sample = indoc::indoc!(
            "199
            200
            208
            210
            200
            207
            240
            269
            260
            263"
        );

        assert_eq!(part1(&input_generator(sample)), 7);
        assert_eq!(part2(&input_generator(sample)), 5);
    }
}
