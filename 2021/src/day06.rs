#[aoc_generator(day06)]
fn parse(input: &str) -> Vec<usize> {
    input
        .trim()
        .split(',')
        .map(|c| c.parse().unwrap())
        .collect()
}

fn count_lanternfishes(fishes: &Vec<usize>, ndays: usize) -> u64 {
    let mut queue: Vec<u64> = fishes.iter().fold(vec![0; 9], |mut map, &n| {
        map[n] += 1;
        map
    });

    for i in 0..ndays {
        queue[(i + 7) % 9] += queue[i % 9];
    }

    queue.iter().map(|&n| n as u64).sum()
}

#[aoc(day06, part1)]
fn part1(fishes: &Vec<usize>) -> u64 {
    count_lanternfishes(fishes, 80)
}

#[aoc(day06, part2)]
fn part2(fishes: &Vec<usize>) -> u64 {
    count_lanternfishes(fishes, 256)
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
        let input = vec![3, 4, 3, 1, 2];
        assert_eq!(count_lanternfishes(&input, 18), 26);
        assert_eq!(part1(&input), 5934);
        assert_eq!(part2(&input), 26984457539);
    }
}
