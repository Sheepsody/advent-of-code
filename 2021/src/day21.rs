use itertools::Itertools;

#[aoc_generator(day21)]
fn parse(input: &str) -> (usize, usize) {
    input
        .lines()
        .map(|l| l.split_once(": ").unwrap().1.parse().unwrap())
        .collect_tuple()
        .unwrap()
}

#[aoc(day21, part1)]
fn part1(&(a, b): &(usize, usize)) -> usize {
    let (mut pos, mut score) = ([a, b], [0, 0]);

    for turn in 0.. {
        let player = turn % 2;

        let dice = turn * 3 + 1;
        let roll = (dice * 3 + 3 + pos[player] - 1) % 10 + 1;

        pos[player] = roll;
        score[player] += roll;

        if score[player] >= 1000 {
            return score[(player + 1) % 2] * (turn + 1) * 3;
        }
    }

    return 0;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse() {
        let sample = indoc::indoc!(
            "Player 1 starting position: 4
            Player 2 starting position: 8"
        );
        assert_eq!(parse(sample), (4, 8));
        assert_eq!(part1(&parse(sample)), 739785);
    }
}
