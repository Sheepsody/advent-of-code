#[aoc_generator(day07)]
fn parse(input: &str) -> Vec<i32> {
    input
        .trim()
        .split(',')
        .map(|c| c.parse().unwrap())
        .collect()
}

#[aoc(day07, part1)]
fn part1(crabs: &Vec<i32>) -> i32 {
    let median = crabs[crabs.len() / 2];
    crabs.iter().map(|x| (x - median).abs()).sum()
}

#[aoc(day07, part2)]
fn part2(crabs: &Vec<i32>) -> i32 {
    let target: i32 = crabs.iter().sum::<i32>() / crabs.len() as i32;
    (target..)
        .take(2)
        .map(|x| {
            crabs
                .iter()
                .map(|n| {
                    let d = (n - x).abs();
                    d * (d + 1) / 2
                })
                .sum()
        })
        .min()
        .unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parsing() {
        assert_eq!(parse("1,2\n"), vec![1, 2]);
    }

    #[test]
    fn sample1() {
        let input = "16,1,2,0,4,2,7,1,2,14";
        assert_eq!(part1(&parse(input)), 37);
        assert_eq!(part2(&parse(input)), 168);
    }
}
